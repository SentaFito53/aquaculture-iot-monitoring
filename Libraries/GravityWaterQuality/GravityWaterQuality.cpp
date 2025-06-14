#include "GravityWaterQuality.h"
#include <EEPROM.h>

// Constructor
GravityWaterQuality::GravityWaterQuality() {
  tdsPin = 35;
  temperature = 25.0;
  aref = 3.3;
  adcRange = 4096.0;
  kValueAddress = 8;
  kValue = 1.0;

  phValue = 7.0;
  acidVoltage = 2032.44;
  neutralVoltage = 1500.0;
  phVoltage = 1500.0;  // default
  phTemperature = 25.0;
  phInputVoltage = 1500.0;

  cmdReceivedBufferIndex = 0;
  memset(cmdReceivedBuffer, 0, sizeof(cmdReceivedBuffer));
}

// Setters
void GravityWaterQuality::setTDSPin(int pin) { tdsPin = pin; }

void GravityWaterQuality::setTemperature(float temp) {
  temperature = temp;
  phTemperature = temp;
}

void GravityWaterQuality::setAref(float value) { aref = value; }

void GravityWaterQuality::setAdcRange(float range) { adcRange = range; }

void GravityWaterQuality::setKvalueAddress(int address) { kValueAddress = address; }

void GravityWaterQuality::setPHPin(int pin) {
  // optional
}

void GravityWaterQuality::begin() {
  pinMode(tdsPin, INPUT);
  EEPROM.begin(512);
  readKValues();

  EEPROM.get(0x00, neutralVoltage);
  EEPROM.get(0x04, acidVoltage);

  if (isnan(neutralVoltage) || neutralVoltage < 100) {
    neutralVoltage = 1500.0;
    }

  if (isnan(acidVoltage) || acidVoltage < 100) {
    acidVoltage = 2032.44;
    }
  }

// TDS
void GravityWaterQuality::readKValues() {
  EEPROM_read(kValueAddress, kValue);
  if (EEPROM.read(kValueAddress) == 0xFF) {
    kValue = 1.0;
    EEPROM_write(kValueAddress, kValue);
    EEPROM.commit();
  }
}

void GravityWaterQuality::updateTDS() {
  analogValue = analogRead(tdsPin);
  voltage = analogValue / adcRange * aref;
  ecValue = (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage) * kValue;
  ecValue25 = ecValue / (1.0 + 0.02 * (temperature - 25.0));
  tdsValue = ecValue25 * TdsFactor;
}

// pH
void GravityWaterQuality::updatePH(float voltage) {
  phInputVoltage = voltage;
  updatePH();  // hitung pH berdasarkan tegangan baru
}

void GravityWaterQuality::updatePH() {
  float slope = (7.0 - 4.0) / ((neutralVoltage - 1500.0) / 3.0 - (acidVoltage - 1500.0) / 3.0);
  float intercept = 7.0 - slope * (neutralVoltage - 1500.0) / 3.0;
  phValue = slope * (phInputVoltage - 1500.0) / 3.0 + intercept;
}

float GravityWaterQuality::getPHValue() {
  updatePH();
  return phValue;
}

float GravityWaterQuality::getTDSValue() {
  return tdsValue;
}

float GravityWaterQuality::getECValue() {
  return ecValue25;
}

void GravityWaterQuality::setPHVoltageInput(float v) {
  phInputVoltage = v;
}

// Serial Calibration Handling
void GravityWaterQuality::processSerialCommands() {
  if (cmdSerialDataAvailable()) {
    byte mode = cmdParse();
    if (mode >= 1 && mode <= 3) {
      ecCalibration(mode);
    } else if (mode >= 4 && mode <= 6) {
      phCalibration(mode);
    }
  }
}

bool GravityWaterQuality::cmdSerialDataAvailable() {
  char cmdReceivedChar;
  static unsigned long cmdReceivedTimeOut = millis();
  while (Serial.available() > 0) {
    if (millis() - cmdReceivedTimeOut > 500U) {
      cmdReceivedBufferIndex = 0;
      memset(cmdReceivedBuffer, 0, ReceivedBufferLength + 1);
    }
    cmdReceivedTimeOut = millis();
    cmdReceivedChar = Serial.read();
    if (cmdReceivedChar == '\n' || cmdReceivedBufferIndex == ReceivedBufferLength) {
      cmdReceivedBuffer[cmdReceivedBufferIndex] = 0;
      cmdReceivedBufferIndex = 0;
      strupr(cmdReceivedBuffer);
      return true;
    } else {
      cmdReceivedBuffer[cmdReceivedBufferIndex++] = cmdReceivedChar;
    }
  }
  return false;
}

byte GravityWaterQuality::cmdParse() {
  if (strstr(cmdReceivedBuffer, "ENTERPH")) return 4;
  else if (strstr(cmdReceivedBuffer, "CALPH")) return 5;
  else if (strstr(cmdReceivedBuffer, "EXITPH")) return 6;
  else if (strstr(cmdReceivedBuffer, "ENTER")) return 1;
  else if (strstr(cmdReceivedBuffer, "CAL:")) return 2;
  else if (strstr(cmdReceivedBuffer, "EXIT")) return 3;
  return 0;
}

// EC Calibration
void GravityWaterQuality::ecCalibration(byte mode) {
  static bool enterCalibrationFlag = false;
  static bool ecCalibrationFinish = false;
  float rawECsolution, KValueTemp;
  char *cmdReceivedBufferPtr;

  switch (mode) {
    case 1:
      enterCalibrationFlag = true;
      ecCalibrationFinish = false;
      Serial.println(F("\n>>>Enter Calibration Mode<<<"));
      Serial.println(F(">>>Put probe into standard buffer solution<<<\n"));
      break;

    case 2:
      cmdReceivedBufferPtr = strstr(cmdReceivedBuffer, "CAL:");
      if (cmdReceivedBufferPtr) {
        cmdReceivedBufferPtr += strlen("CAL:");
        rawECsolution = strtod(cmdReceivedBufferPtr, NULL) / TdsFactor;
        rawECsolution *= (1.0 + 0.02 * (temperature - 25.0));
        if (enterCalibrationFlag) {
          KValueTemp = rawECsolution / (133.42 * voltage * voltage * voltage - 255.86 * voltage * voltage + 857.39 * voltage);
          if ((rawECsolution > 0) && (KValueTemp > 0)) {
            kValue = KValueTemp;
            ecCalibrationFinish = true;
            Serial.println(F("\n>>>Calibration Successful!<<<"));
            Serial.print(F("kValue = "));
            Serial.println(kValue);
          } else {
            Serial.println(F("\n>>>Calibration Failed! Invalid values!<<<"));
          }
        } else {
          Serial.println(F("\n>>>Enter calibration mode first!<<<"));
        }
      }
      break;

    case 3:
      if (enterCalibrationFlag) {
        if (ecCalibrationFinish) {
          EEPROM_write(kValueAddress, kValue);
	  EEPROM.commit();
          Serial.println(F("\n>>>Calibration Data Saved<<<"));
        } else {
          Serial.println(F("\n>>>No valid calibration data to save!<<<"));
        }
        enterCalibrationFlag = false;
        ecCalibrationFinish = false;
      }
      break;
  }
}

// pH Calibration
void GravityWaterQuality::phCalibration(byte mode) {
  static bool enterPHCalibrationFlag = false;
  static bool phCalibrationFinish = false;
  float voltageValue = phInputVoltage;

  switch (mode) {
    case 4:
      enterPHCalibrationFlag = true;
      phCalibrationFinish = false;
      Serial.println(F("\n>>>Enter PH Calibration Mode<<<"));
      Serial.println(F(">>>Put probe into pH buffer solution<<<\n"));
      break;

    case 5:
      if (enterPHCalibrationFlag) {
        if (voltageValue > 1322 && voltageValue < 1678) {
          neutralVoltage = voltageValue;
          Serial.println(F("\n>>>PH Calibration for pH7 buffer Successful!<<<"));
          Serial.print(F("acidVoltage = "));
          Serial.println(acidVoltage);
        } else if (voltageValue > 1854 && voltageValue < 2210) {
          acidVoltage = voltageValue;
          Serial.println(F("\n>>>PH Calibration for pH4 buffer Successful!<<<"));
          Serial.print(F("neutralVoltage = "));
          Serial.println(neutralVoltage);
        }
        phCalibrationFinish = true;
      } else {
        Serial.println(F("\n>>>Invalid voltage or not in calibration mode<<<"));
      }
      break;

    case 6:
      if (enterPHCalibrationFlag) {
        if (phCalibrationFinish) {
          EEPROM_write(0x00, neutralVoltage);
          EEPROM_write(0x04, acidVoltage);
          EEPROM.commit();
          Serial.println(F("\n>>>PH Calibration Data Saved<<<"));
        } else {
          Serial.println(F("\n>>>No valid PH calibration data to save!<<<"));
        }
        enterPHCalibrationFlag = false;
        phCalibrationFinish = false;
      }
      break;
  }
}

// Helper
char* GravityWaterQuality::strupr(char* str) {
  char* ptr = str;
  while (*ptr) {
    if (*ptr >= 'a' && *ptr <= 'z') *ptr -= 32;
    ptr++;
  }
  return str;
}
