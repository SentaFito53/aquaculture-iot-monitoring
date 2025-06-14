#define DO_PIN 34              // analog Pin
#define VREF 3.3               // reference voltage
#define ADC_RES 4095.0         // ESP32 ADC Resolution
#define DO_K 4.236             // Constanta calib result expose-to-air

void setup() {
  Serial.begin(115200);
}

void loop() {
  int adcValue = analogRead(DO_PIN);
  float voltage = adcValue * VREF / ADC_RES;

  float doValue = voltage * DO_K;

  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | DO: ");
  Serial.print(doValue, 2);
  Serial.println(" mg/L");

  delay(1000);
}
