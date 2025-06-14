#ifndef DO_CALIBRATOR_H
#define DO_CALIBRATOR_H

#include <Arduino.h>
#include <EEPROM.h>

class DOCalibrator {
  public:
    DOCalibrator(uint8_t analogPin, float vref = 3.3, float adcRes = 4095.0);

    void begin();
    float readDO();
    void handleSerialCalibration();  // Panggil di loop
    float getVoltage();
    float getK();

  private:
    uint8_t _pin;
    float _vref;
    float _adcRes;
    float _doK;

    const int EEPROM_ADDR = 0; // alamat penyimpanan
    void loadCalibration();
    void saveCalibration(float newK);
};

#endif
