# aquaculture-iot-monitoring
This project is an IoT-based water quality monitoring system designed for aquaculture ponds. Built using the ESP32 microcontroller, it continuously measures Dissolved Oxygen (DO), pH, and Total Dissolved Solids (TDS), sends the data in real-time to Firebase Realtime Database, and logging to Spreadsheet for analysis.

🌊 IoT Pond Water Quality Monitoring (DO, pH, TDS)
This project is an ESP32-based IoT system designed to monitor pond water quality in real-time using Dissolved Oxygen (DO), pH, and TDS sensors. The data is displayed on a 20x4 LCD and sent to Firebase Realtime Database.

📦 Key Features
🚰 Monitoring Dissolved Oxygen (DO) levels (mg/L)
🌡️ Monitoring pH value (water acidity)
💧 Monitoring TDS (Total Dissolved Solids) (ppm)
📟 Displaying data on a 20x4 I2C LCD
☁️ Integration with Firebase Realtime Database
🛠️ Sensor calibration via Serial commands
💾 Calibration values stored in EEPROM (retained after restart)

🔧 Hardware Used
ESP32 Dev Board
pH Sensor (Gravity Analog pH Meter by DFRobot)
TDS Sensor (Gravity Analog TDS Meter by DFRobot)
DO Sensor (Gravity Analog Dissolved Oxygen by DFRobot)
I2C 20x4 LCD
