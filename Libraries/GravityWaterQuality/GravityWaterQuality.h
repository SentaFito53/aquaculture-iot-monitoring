#ifndef _GRAVITY_WATER_QUALITY_H_
#define _GRAVITY_WATER_QUALITY_H_

#include <Arduino.h>
#include <EEPROM.h>

// EEPROM helper macros
#define EEPROM_write(address, p) {int i = 0; byte *pp = (byte*)&(p); for(; i < sizeof(p); i++) EEPROM.put(address + i, pp[i]);}
#define EEPROM_read(address, p)  EEPROM.get(address, p)


#define ReceivedBufferLength 32

// TDS calibration factor constant
#define TdsFactor 0.5  // default factor, bisa disesuaikan

class GravityWaterQuality {
  public:
    GravityWaterQuality();

    // Set input nilai tegangan pH jika ingin langsung set (optional)
    void setPHVoltageInput(float v);

    // Pin setup dan parameter lain
    void setTDSPin(int pin);
    void setPHPin(int pin);   // Pin analog sensor pH, jika pakai pin khusus
    void setTemperature(float temp);  // suhu lingkungan
    void setAref(float value);
    void setAdcRange(float range);
    void setKvalueAddress(int address);

    void begin();

    // Update sensor data dengan membaca analog (update internal state)
    void updateTDS();
    void updatePH(float voltage);
    void updatePH();

    // Dapatkan nilai dari sensor yang telah diupdate
    float getTDSValue();
    float getECValue();
    float getPHValue();

    // Proses kalibrasi via serial (jika ada komunikasi kalibrasi)
    void processSerialCommands();

  private:
    // --- Variabel TDS ---
    float phInputVoltage;  // voltage input dari luar, untuk pH jika ingin di-set manual
    int tdsPin;
    int phPin;
    float temperature;
    float aref;
    float adcRange;
    int kValueAddress;
    float kValue;

    int analogValue;
    float voltage;
    float ecValue;
    float ecValue25;
    float tdsValue;

    // --- Variabel pH ---
    float phValue;
    float acidVoltage;
    float neutralVoltage;
    float phVoltage;
    float phTemperature;

    // Buffer serial command
    char cmdReceivedBuffer[ReceivedBufferLength + 1];
    int cmdReceivedBufferIndex;

    // Internal helper methods
    void readKValues();
    bool cmdSerialDataAvailable();
    byte cmdParse();

    // Metode kalibrasi untuk TDS dan pH
    void ecCalibration(byte mode);
    void phCalibration(byte mode);

    // Helper fungsi
    char* strupr(char* str);
};

#endif
