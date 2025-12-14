# 🌫️ ESP32 Air Quality Monitoring System

A comprehensive IoT solution for real-time indoor air quality monitoring. This project integrates an ESP32 microcontroller with a modern, responsive web dashboard to track Temperature, Humidity, and CO2 levels, complete with an automated alert system.

**[🌐 Live Demo](https://project-air-quality-iot.onrender.com/)**

---

## 🚀 Features

-   **Real-time Monitoring**: Live streaming of CO2 (ppm), Temperature (°C), and Humidity (%) data via WebSockets.
-   **Interactive Dashboard**: A clean, "Glassmorphism" UI design with dynamic charts and live status indicators.
-   **Smart Alerts**: 
    -   **Automated**: Buzzer and Red LED activate automatically when CO2 levels exceed **1400 ppm**.
    -   **Manual**: Control the Buzzer remotely directly from the Web Interface.
-   **Dual Feedback**: Visual feedback via LCD I2C screen on the device and the Web App.

## 🛠️ Tech Stack

### Hardware
-   **Microcontroller**: ESP32 Dev Module
-   **Sensors**: 
    -   **MQ-135**: Air Quality (CO2/Gas)
    -   **DHT11**: Temperature & Humidity
-   **Output**: 
    -   LCD 1602 (I2C)
    -   Active Buzzer
    -   LEDs (Red/Green)

### Software & Protocols
-   **Firmware**: C++ (Arduino Framework)
-   **Communication**: MQTT (HiveMQ Cloud) for device-server comms, WebSockets (Socket.io) for realtime frontend updates.
-   **Backend**: Node.js, Express.
-   **Frontend**: HTML5, Modern CSS3 (Variables, Flexbox/Grid), Chart.js.

---

## 📦 Installation & Setup

### 1. Hardware Setup (ESP32)

**Wiring Diagram**:
-   **MQ135**: Analog Pin 34
-   **DHT11**: Pin 4
-   **Buzzer**: Pin 27
-   **Green LED**: Pin 16
-   **Red LED**: Pin 23
-   **LCD I2C**: SDA (21 or 32), SCL (22 or 33) *(Check code for specific pin config)*

**Firmware**:
1.  Open `ESP32_AIR_QUALITY.ino` in Arduino IDE.
2.  Install required libraries:
    -   `PubSubClient` (by Nick O'Leary)
    -   `ArduinoJson` (by Benoit Blanchon)
    -   `DHT sensor library` (by Adafruit)
    -   `LiquidCrystal I2C`
3.  Update WiFi credentials (`ssid`, `password`) and MQTT config if necessary.
4.  Upload to ESP32.

### 2. Web Application Setup

**Prerequisites**: [Node.js](https://nodejs.org/) installed.

1.  Navigate to the web app directory:
    ```bash
    cd web-app
    ```
2.  Install dependencies:
    ```bash
    npm install
    ```
3.  Start the server:
    ```bash
    npm start
    ```
    The server will run on `http://localhost:3000` (or the PORT defined in environment).

---

## 📖 Usage Guide

1.  **Power on** the ESP32. It will connect to WiFi and the MQTT broker.
2.  Open the **Web Dashboard** (Local or Demo link).
3.  **Monitoring**: Observe the gauges for current metrics and the chart for historical CO2 trends.
4.  **Alerts**: 
    -   If CO2 is safe (< 1400 ppm), the Green LED is ON.
    -   If CO2 is dangerous (>= 1400 ppm), the Red LED turns ON, and the Buzzer alarms. The Web App "Buzzer" button will also sync to "Active" state.
5.  **Remote Control**: Click **"Activate Buzzer"** on the dashboard to manually trigger the alarm on the ESP32 device.

---

## 📂 Project Structure

```
├── CODE
│   ├── .ino
│   │   └── ESP32_AIR_QUALITY    # ESP32 Firmware Source Code
│   └── web-app
│       ├── public              # Frontend (HTML, CSS, JS)
│       ├── mqttClient.js       # MQTT Handler
│       └── server.js           # Node.js Server Entry
```

## 👨‍💻 Author

Developed for **Internet of Things (IoT)** Course.
semester 4-1.
