#include <GravityWaterQuality.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "addons/TokenHelper.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ====================== WiFi and Firebase Configuration =======================
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define API_KEY ""
#define DATABASE_URL ""
#define USER_EMAIL ""
#define USER_PASSWORD ""

// ====================== Sensor Configuration =======================
#define TDS_PIN 35
#define PH_PIN 36
#define DO_PIN 34

#define VREF 3300
#define ADC_RES 4095.0
#define DO_K 7.277533
float temperature = 25.0;

// ====================== Firebase and Sensor Objects =======================
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
GravityWaterQuality gwq;
LiquidCrystal_I2C lcd(0x27, 20, 4);

float doValue, tdsValue, phValue;
String waterStatus = "-";

// ====================== Fuzzy Logic Membership Functions =======================
float lowPH(float pH) {
  if (pH <= 6) return 1.0;
  else if (pH > 6 && pH < 6.5) return (6.5 - pH) / 0.5;
  return 0.0;
}

float normalPH(float pH) {
  if (pH >= 6.5 && pH <= 8.5) return 1.0;
  else if (pH > 6 && pH < 6.5) return (pH - 6) / 0.5;
  else if (pH > 8.5 && pH < 9) return (9 - pH) / 0.5;
  return 0.0;
}

float highPH(float pH) {
  if (pH >= 9) return 1.0;
  else if (pH > 8.5 && pH < 9) return (pH - 8.5) / 0.5;
  return 0.0;
}

float lowTDS(float TDS) {
  if (TDS <= 150) return 1.0;
  else if (TDS > 150 && TDS < 200) return (200 - TDS) / 50.0;
  return 0.0;
}

float highTDS(float TDS) {
  if (TDS >= 200) return 1.0;
  else if (TDS > 150 && TDS < 200) return (TDS - 150) / 50.0;
  return 0.0;
}

float lowDO(float DO) {
  if (DO <= 3) return 1.0;
  else if (DO > 3 && DO < 4) return (4 - DO) / 1.0;
  return 0.0;
}

float normalDO(float DO) {
  if (DO >= 4 && DO <= 8) return 1.0;
  else if (DO > 3 && DO < 4) return (DO - 3) / 1.0;
  else if (DO > 8 && DO < 9) return (9 - DO) / 1.0;
  return 0.0;
}

float highDO(float DO) {
  if (DO >= 9) return 1.0;
  else if (DO > 8 && DO < 9) return (DO - 8) / 1.0;
  return 0.0;
}

// ====================== Fuzzy Logic Evaluation =======================
float evaluateWaterQuality(float pH, float TDS, float DO) {
  float rule1 = min(min(lowPH(pH), lowTDS(TDS)), lowDO(DO));
  float rule2 = min(min(lowPH(pH), lowTDS(TDS)), highDO(DO));
  float rule3 = min(min(lowPH(pH), highTDS(TDS)), lowDO(DO));
  float rule4 = min(min(lowPH(pH), highTDS(TDS)), highDO(DO));
  float rule5 = min(min(normalPH(pH), lowTDS(TDS)), lowDO(DO));
  float rule6 = min(min(normalPH(pH), lowTDS(TDS)), highDO(DO));
  float rule7 = min(min(normalPH(pH), highTDS(TDS)), lowDO(DO));
  float rule8 = min(min(normalPH(pH), highTDS(TDS)), highDO(DO));
  float rule9 = min(min(highPH(pH), lowTDS(TDS)), lowDO(DO));
  float rule10 = min(min(highPH(pH), lowTDS(TDS)), highDO(DO));
  float rule11 = min(min(highPH(pH), highTDS(TDS)), lowDO(DO));
  float rule12 = min(min(highPH(pH), highTDS(TDS)), highDO(DO));

  float totalWeight = rule1 + rule2 + rule3 + rule4 + rule5 + rule6 +
                      rule7 + rule8 + rule9 + rule10 + rule11 + rule12;

  float weightedSum =
      (rule1 * 25) + (rule2 * 75) + (rule3 * 25) + (rule4 * 25) +
      (rule5 * 25) + (rule6 * 75) + (rule7 * 25) + (rule8 * 75) +
      (rule9 * 25) + (rule10 * 75) + (rule11 * 25) + (rule12 * 75);

  if (totalWeight == 0) return 0;
  return weightedSum / totalWeight;
}

void fuzzyEvaluation(float pH, float TDS, float DO) {
  float score = evaluateWaterQuality(pH, TDS, DO);
  Serial.print("Water Quality Score: ");
  Serial.println(score);

  if (score < 50) {
    waterStatus = "Poor";
  } else {
    waterStatus = "Good";
  }
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  Wire.begin(21, 22);  // SDA, SCL for ESP32

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pond Monitoring");

  gwq.setTDSPin(TDS_PIN);
  gwq.setPHPin(PH_PIN);
  gwq.setTemperature(temperature);
  gwq.setAref(3.3);
  gwq.setAdcRange(ADC_RES);
  gwq.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.setCursor(0, 1);
  lcd.print("WiFi Connected     ");
  Serial.println("\nWiFi connected");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectNetwork(true);
  Firebase.begin(&config, &auth);

  while ((auth.token.uid) == "") {
    delay(1000);
    Serial.print(".");
  }

  lcd.setCursor(0, 1);
  lcd.print("Firebase Connected ");
  delay(1000);
  lcd.clear();
}

void loop() {
  int rawDO = analogRead(DO_PIN);
  float voltageDO = rawDO * VREF / ADC_RES;
  doValue = voltageDO * (DO_K / 1000);

  gwq.updateTDS();
  int analogPH = analogRead(PH_PIN);
  float voltagePH = (float)analogPH / ADC_RES * VREF;
  gwq.setPHVoltageInput(voltagePH);

  phValue = gwq.getPHValue();
  tdsValue = gwq.getTDSValue();

  fuzzyEvaluation(phValue, tdsValue, doValue);

  Serial.print("DO: "); Serial.print(doValue, 2);
  Serial.print(" | TDS: "); Serial.print(tdsValue, 0);
  Serial.print(" | pH: "); Serial.print(phValue, 2);
  Serial.print(" | Status: "); Serial.println(waterStatus);

  lcd.setCursor(0, 0);
  lcd.print("pH : "); lcd.print(phValue, 2); lcd.print("   ");
  lcd.setCursor(0, 1);
  lcd.print("DO : "); lcd.print(doValue, 2); lcd.print(" mg/L   ");   
  lcd.setCursor(0, 2);
  lcd.print("TDS: "); lcd.print(tdsValue, 0); lcd.print(" ppm     ");
  lcd.setCursor(0, 3);
  lcd.print("Status: "); lcd.print(waterStatus); lcd.print("     ");

  if (Firebase.ready()) {
    Firebase.setFloat(fbdo, "/pondmonitor/do", doValue);
    Firebase.setFloat(fbdo, "/pondmonitor/tds", tdsValue);
    Firebase.setFloat(fbdo, "/pondmonitor/ph", phValue);
    Firebase.setString(fbdo, "/pondmonitor/status", waterStatus);
  }

  gwq.processSerialCommands();
  delay(2000);
}
