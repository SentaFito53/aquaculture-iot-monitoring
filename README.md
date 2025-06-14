# 🌊 Water Quality Monitoring (ESP32 + Firebase)

This project is an IoT-based water quality monitoring system using ESP32. It measures **pH**, **TDS**, and **Dissolved Oxygen (DO)** levels in real-time, displays the data on an LCD, and uploads it to Firebase Realtime Database.

---

## 📦 Key Features

- 🚰 Monitors **Dissolved Oxygen (DO)** (mg/L)
- 🌡️ Monitors **pH** (acidity of water)
- 💧 Monitors **TDS** (Total Dissolved Solids) (ppm)
- 📟 Displays data on **I2C 20x4 LCD**
- ☁️ Sends data to **Firebase Realtime Database**
- 🛠️ Supports **sensor calibration via Serial commands**
- 💾 Stores calibration values in **EEPROM** (retained after reset)

---

## 🔧 Hardware Used

- ESP32 Dev Board
- pH Sensor (Gravity Analog pH Meter)
- TDS Sensor (Gravity Analog TDS Meter)
- DO Sensor (Gravity Analog Dissolved Oxygen)
- 20x4 LCD I2C
- Breadboard and jumper wires

---

## ⚙️ Sensor Calibration Guide

### 🔹 1. pH & TDS Calibration (via `main.ino`)

> Calibration is done via the **Serial Monitor** while running `main.ino`.

#### 🧪 How to calibrate pH sensor:
1. Prepare pH buffer solutions (usually pH 4.0 and pH 7.0).
2. Open Serial Monitor at **115200 baud**.
3. Follow the on-screen instructions.
4. Example command:
5. Repeat for other pH values if necessary.

#### 💧 How to calibrate TDS sensor:
1. Use a TDS calibration solution (e.g., 707 ppm).
2. Open Serial Monitor.
3. Type the command:

> The calibration constants are automatically saved to **EEPROM** and persist after reset.

---

### 🔹 2. DO Sensor Calibration (via `DOCalibrator.ino`)

> Calibration for the DO sensor is handled **separately** using the `DOCalibrator.ino` sketch.

#### 🌬️ DO Calibration Method: *Expose-to-Air*
1. Make sure the DO sensor is **dry and exposed to air**.
2. Upload and run `DOCalibrator.ino`.
3. Open Serial Monitor at **115200 baud**.
4. Enter the command:
> 8.26 is the typical DO value in air at sea level and 25°C.

5. After calibration, the constant is saved to **EEPROM**.
6. You can now use the `main.ino` sketch to monitor DO along with pH and TDS.

---

## 📁 File Structure

