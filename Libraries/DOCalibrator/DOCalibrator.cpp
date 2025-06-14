#include "DOCalibrator.h"

DOCalibrator::DOCalibrator(uint8_t analogPin, float vref, float adcRes) {
  _pin = analogPin;
  _vref = vref;
  _adcRes = adcRes;
}

void DOCalibrator::begin() {
  EEPROM.begin(32);
  loadCalibration();
}

float DOCalibrator::getVoltage() {
  int adc = analogRead(_pin);
  return (adc * _vref) / _adcRes;
}

float DOCalibrator::readDO() {
  float voltage = getVoltage();
  return voltage * _doK;
}

void DOCalibrator::loadCalibration() {
  EEPROM.get(EEPROM_ADDR, _doK);
  if (isnan(_doK) || _doK <= 0.01 || _doK > 10000) {
    _doK = 1.0; // default
  }
}

void DOCalibrator::saveCalibration(float newK) {
  _doK = newK;
  EEPROM.put(EEPROM_ADDR, _doK);
  EEPROM.commit();
}

float DOCalibrator::getK() {
  return _doK;
}

void DOCalibrator::handleSerialCalibration() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.startsWith("CAL")) {
      // Format: CAL:<DO_jenuh>
      float doSaturasi = input.substring(4).toFloat();
      float voltage = getVoltage();
      float newK = doSaturasi / voltage;

      saveCalibration(newK);

      Serial.println("✅ Kalibrasi berhasil:");
      Serial.print("Tegangan saat ini: ");
      Serial.print(voltage, 3);
      Serial.print(" V | DO jenuh: ");
      Serial.print(doSaturasi);
      Serial.print(" mg/L -> Konstanta K = ");
      Serial.println(newK, 4);
    } else if (input == "K?") {
      Serial.print("Konstanta K saat ini: ");
      Serial.println(_doK, 4);
    } else {
      Serial.println("❌ Format salah. Gunakan: CAL:<DO_jenuh> atau K?");
    }
  }
}
