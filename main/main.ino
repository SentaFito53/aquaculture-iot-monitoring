#include <GravityWaterQuality.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include "addons/TokenHelper.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ====================== WiFi and Firebase Configuration =======================
#define WIFI_SSID "Polibatam"
#define WIFI_PASSWORD ""
#define API_KEY "AIzaSyDNojusrLaKfrNaw9wtlBPjyLOibCrolnw"
#define DATABASE_URL "https://monitoringkolam-55ac7-default-rtdb.firebaseio.com/"
#define USER_EMAIL "ptfamsgroupsejahtera@gmail.com"
#define USER_PASSWORD "Aremania0778"

// ====================== Sensor Configuration =======================
#define TDS_PIN 35
#define PH_PIN 36
#define DO_PIN 34

#define VREF 3300
#define ADC_RES 4095.0
#define DO_K 7.277533   // <- Hasil kalibrasi expose-to-air (ganti sesuai nilai kamu)
float temperature = 25.0;

// ====================== Firebase & Sensor Object=======================
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
GravityWaterQuality gwq;
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ====================== Sensor Variable =======================
float doValue, tdsValue, phValue;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  Wire.begin(21, 22); // SDA, SCL for ESP32

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Monitoring Kolam");

  //GWQ Sensor(TDS dan pH)
  gwq.setTDSPin(TDS_PIN);
  gwq.setPHPin(PH_PIN);
  gwq.setTemperature(temperature);
  gwq.setAref(3.3);
  gwq.setAdcRange(ADC_RES);
  gwq.begin();

  // WiFi Conection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0, 1);
  lcd.print("WiFi connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.setCursor(0, 1);
  lcd.print("WiFi connected     ");
  Serial.println("\nWiFi connected");

  //  Firebase Configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  Firebase.reconnectNetwork(true);
  Firebase.begin(&config, &auth);

  // Wait for autentication
  while ((auth.token.uid) == "") {
    delay(1000);
    Serial.print(".");
  }
  lcd.setCursor(0, 1);
  lcd.print("Firebase Connected ");
  Serial.println("\nFirebase connected!");
  lcd.clear();
}

void loop() {
  // ======== Read DO Sensor =========
  int rawDO = analogRead(DO_PIN);
  float voltageDO = rawDO * VREF / ADC_RES;
  doValue = voltageDO * (DO_K / 1000); 

  // ======== Read TDS and pH =========
  gwq.updateTDS();

  int analogPH = analogRead(PH_PIN);
  float voltagePH = (float)analogPH / ADC_RES * VREF;
  gwq.setPHVoltageInput(voltagePH);

  phValue = gwq.getPHValue();
  tdsValue = gwq.getTDSValue();

  // ======== Serial Monitor =========
  Serial.print("DO: "); Serial.print(doValue, 2);
  Serial.print(" mg/L | TDS: "); Serial.print(tdsValue, 0);
  Serial.print(" ppm | pH: "); Serial.println(phValue, 2);

  // ======== LCD Display =========
  lcd.setCursor(0, 0);
  lcd.print("Monitoring Kolam");
  lcd.setCursor(0, 1);
  lcd.print("pH  : "); lcd.print(phValue, 2); lcd.print("       ");

  lcd.setCursor(0, 2);
  lcd.print("TDS : "); lcd.print(tdsValue, 0); lcd.print(" ppm     ");

  lcd.setCursor(0, 3);
  lcd.print("DO  : "); lcd.print(doValue, 2); lcd.print(" mg/L ");

  // Status pH Netral/Asam/Basa
  lcd.setCursor(14, 3);
  if (phValue < 6.5) lcd.print("Asam ");
  else if (phValue > 8.5) lcd.print("Basa ");
  else lcd.print("Netrl");

  // ======== Send to Firebase =========
  if (Firebase.ready()) {
    Firebase.setFloat(fbdo, "/monitoringkolam/do", doValue);
    Firebase.setFloat(fbdo, "/monitoringkolam/tds", tdsValue);
    Firebase.setFloat(fbdo, "/monitoringkolam/ph", phValue);
  }

  // pH and TDS Calib from serial
  gwq.processSerialCommands();

  delay(1000);
}
