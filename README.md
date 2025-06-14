# 🌊 Water Quality Monitoring (ESP32 + Firebase)

This project is an IoT-based water quality monitoring system using ESP32. It measures **pH**, **TDS**, and **Dissolved Oxygen (DO)** levels in real-time, displays the data on an LCD, and uploads it to Firebase Realtime Database.

---

## 📦 Key Features

- 🚰 Monitors **Dissolved Oxygen (DO)** (mg/L)
- 🌡️ Monitors **pH** (acidity of water)
- 💧 Monitors **TDS** (Total Dissolved Solids) (ppm)
- 📟 Displays data on **I2C 20x4 LCD**
- ☁️ Sends data to **Firebase Realtime Database**
- 📊 **Logs data to Google Spreadsheet** via Google Apps Script webhook
- 🛠️ Supports **sensor calibration via Serial commands**
- 💾 Stores calibration values in **EEPROM** (retained after reset)

---

## 🔧 Hardware Used

- ESP32 Dev Board
- pH Sensor (Gravity Analog pH Meter by DFRobot)
- TDS Sensor (Gravity Analog TDS Meter by DFRobot)
- DO Sensor (Gravity Analog Dissolved Oxygen by DFRobot)
- 20x4 LCD I2C

---

## ⚙️ Sensor Calibration Guide

### 🔹 1. pH & TDS Calibration (via [main.ino](https://github.com/SentaFito53/aquaculture-iot-monitoring/blob/main/main/main.ino))

> Calibration is done via the **Serial Monitor** while running `main.ino`.

#### 🧪 How to calibrate pH sensor:
1. Prepare pH buffer solutions (usually pH 4.0 and pH 7.0).
2. Open Serial Monitor at **115200 baud**.
3. Follow the on-screen instructions.
4. Example command: ENTERPH (enter Calib mode), CALPH (read current pH value), and EXITPH (save calib value)
5. Before EXITPH make sure the pH value is stable
6. Repeat for other pH values if necessary.

#### 💧 How to calibrate TDS sensor:
1. Use a TDS calibration solution (e.g., 500 ppm).
2. Open Serial Monitor.
3. Type the command: enter (enter Calib mode), cal:500 (set current TDS Value), and exit (save calib value)

> The calibration constants are automatically saved to **EEPROM** and persist after reset.

---

### 🔹 2. DO Sensor Calibration (via `DOCalibrator.ino`)

> Calibration for the DO sensor is handled **separately** using the `DOCalibrator.ino` sketch.

#### 🌬️ DO Calibration Method: *Expose-to-Air*
1. Make sure the DO sensor is **dry and exposed to air**.
2. Upload and run `DOCalibrator.ino`.
3. Open Serial Monitor at **115200 baud**.
4. Read the voltage value
5. Enter the voltage value to this formula (voltage/8.26)
6. Value from the equation will be entered to DO_K variabel on `main.ino`
> 8.26 is the typical DO value in air at sea level and 25°C.

---

## 📁 File Structure

