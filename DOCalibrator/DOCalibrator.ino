#include <DOCalibrator.h>

DOCalibrator doSensor(34, 3.3, 4095.0);

void setup() {
  Serial.begin(115200);
  doSensor.begin();
  Serial.println("Type CAL:<nilai DO jenuh> for Calib, ex: CAL:8.26");
}

void loop() {
  float voltage = doSensor.getVoltage();
  float doVal = doSensor.readDO();

  Serial.print("Voltage: ");
  Serial.print(voltage, 3);
  Serial.print(" V | DO: ");
  Serial.print(doVal, 2);
  Serial.println(" mg/L");

  doSensor.handleSerialCalibration();

  delay(1000);
}
