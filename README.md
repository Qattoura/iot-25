# README: Weather Monitoring System (Flona v1.0.3)
## Project Overview
Flona v1.0.3 is a smart weather monitoring system designed to detect and record environmental conditions like temperature, humidity, and air quality. It issues alerts when readings exceed predefined safety thresholds and logs data for future analysis. The system includes a web-based user interface for real-time monitoring.

This project aligns with the requirements of designing a weather station capable of measuring and recording environmental conditions while sending alerts during abnormal events​
## Features
### 1. Temperature and Humidity Monitoring:
- Uses a DHT11 sensor to measure temperature and humidity.
- Displays real-time data on a web interface.

### 2. Air Quality and Smoke Detection:
- Monitors air quality using a smoke sensor and raises alerts during hazardous conditions.
### 3. Light Sensor for Environmental Brightness:

- Adjusts external LEDs based on ambient light intensity.
### 4. Servo-Controlled Window:

- Automatically opens/closes based on temperature thresholds.
### 5. Data Logging:

- Records all sensor readings to an SD card with timestamped entries.
### 6. Alerts:

- Sends email notifications during critical events (e.g., fire detection).
### 7. Web Interface:

- Displays real-time temperature and humidity values.
- Shows alerts for abnormal conditions.

## Hardware Components
- ESP32 Development Board: Main controller.
- DHT11: For temperature and humidity measurement.
- Servo Motor: Controls window mechanism.
- Smoke Sensor: Detects smoke or air quality changes.
- Light Sensor: Measures ambient light.
- SD Card Module: Logs environmental data.
- LED Indicators: Signals different system states.
- External Power Supply: Supports all components.

## Software Overview
### 1. Programming Language: Arduino C/C++
### 2. Libraries Used:
- WiFi for ESP32 connectivity.
- ESP Mail Client for email notifications.
- NTPClient for time synchronization.
- Adafruit Sensor and DHT libraries for sensor management.
- ESPAsyncWebServer for the web server.
### 3. Functionality:
- Initializes sensors and components.
- Logs data at regular intervals.
- Serves a web page to display real-time conditions.
- Sends alerts via email during critical events.

## How to Use
### Setup Hardware:
- Connect the ESP32 and sensors as per the circuit diagram.
- Ensure proper wiring for SD card and servo motor.
### Upload Code:
- Load the provided Arduino sketch into the ESP32 using the Arduino IDE.
### Configure WiFi Credentials:
- Replace placeholders with your WiFi SSID and password in the code.
### Access Web Interface:
- After setup, connect to the ESP32's local IP to view the interface.
### Monitor and Record:
- View real-time data and alerts via the web interface.
- Check the SD card for recorded logs.

## Key Functionalities
### 1. Temperature and Humidity:
- Automatically logs readings every 5 seconds.
- Triggers a servo to adjust the window if the temperature exceeds 24°C.
### 2. Air Quality Alerts:
- Activates alerts if smoke sensor readings surpass a threshold.
- Sends an email notification to preconfigured recipients.
### 3. Light Control:
- Controls external LEDs based on ambient light levels.
### 4. Data Logging:
- Logs data in data.txt on the SD card in the format:
Date, Time, Temperature, Humidity
### 5. Web Server:
- Real-time data visualization.
- Displays warnings for critical environmental changes.

## Enhancements and Innovations
### Automated Window Adjustment:
- Maintains environmental comfort through a servo-controlled window.
### Email Alerts:
- Critical alerts ensure user awareness even when off-site.
### Customizable Thresholds:
- Easily configurable limits for alerts in the code.
