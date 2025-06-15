# 🌊 Water Quality Monitoring with ESP32 + Firebase + Fuzzy Logic
Upgrade version from [main.ino](https://github.com/SentaFito53/aquaculture-iot-monitoring/blob/main/main/main.ino) with fuzzy logic
## 📌 Overview

This project uses an **ESP32** microcontroller to monitor water quality using **pH**, **TDS**, and **DO** sensors. Data is displayed on an **I2C 20x4 LCD** and sent to **Firebase Realtime Database**. It features a **Sugeno-type fuzzy logic** system to evaluate water quality as `"Good"` or `"Poor"`.

---

## 🔧 Features

- Real-time sensor readings:
  - pH
  - TDS (ppm)
  - DO (mg/L)
- LCD display with static labels
- Firebase integration
- Fuzzy logic decision making

---

## 🧠 Fuzzy Logic Evaluation

The system uses 12 fuzzy rules combining:
- pH: low, normal, high  
- TDS: low, high  
- DO: low, normal, high

A weighted average determines the score:
- Score < 50 → `Poor`
- Score ≥ 50 → `Good`

---

## 📟 LCD Output Format

pH : 7.32
TDS: 238 ppm
DO : 6.80 mg
Status: Good


---

## ☁️ Firebase Data Structure

```json
{
  "monitoringkolam": {
    "ph": 7.32,
    "tds": 238,
    "do": 6.80,
    "status": "Good"
  }
}
