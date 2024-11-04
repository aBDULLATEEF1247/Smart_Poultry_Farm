Smart Poultry Farm Monitoring System
This project is a smart monitoring and control system for a poultry farm, developed using an ESP32 microcontroller. It continuously monitors and controls environmental parameters such as temperature, humidity, light intensity, and detects intruders. The data is stored in real-time on Firebase and accessible through a GUI for ease of management by farm owners.

Features
Temperature and Humidity Control:

Uses a DHT11 sensor to monitor temperature and humidity.
Activates a fan to reduce temperature when it exceeds 34°C.
Activates a heater when the temperature falls below 25°C.
Controls humidity with a humidifier as needed.
Light Intensity Control:

An LDR sensor monitors light levels.
Activates or deactivates a light relay based on the detected light intensity.
Intruder Detection and Video Surveillance:

PIR sensor detects movement in the farm for intruder detection.
Streams video using ESP32 Cam for real-time surveillance upon detecting an intruder.
Sends a stream address to Firebase for remote viewing.
Data Storage on Firebase:

Firebase is used for real-time data storage and monitoring of farm conditions.
Data includes battery level, temperature, humidity, light status, fan status, heater status, and intruder alerts.
Remote Management GUI:

A graphical user interface (GUI) is available for farm owners to remotely monitor and control environmental parameters via Firebase.
Hardware Requirements
ESP32 Microcontroller
DHT11 Temperature and Humidity Sensor
LDR for Light Intensity Detection
PIR Sensor for Intruder Detection
Relay Modules for Fan, Heater, and Lights
ESP32 Cam for Video Surveillance
Libraries Used
FirebaseESP32: For connecting to Firebase and storing real-time data.
WiFi: To establish internet connectivity.
HTTPClient: For HTTP requests.
DHT: To interface with the DHT11 temperature and humidity sensor.
TokenHelper.h and RTDBHelper.h: Firebase helper libraries.
Installation
Library Setup: Install necessary libraries (FirebaseESP32, WiFi, DHT).
Configuration:
Replace API_KEY and DATABASE_URL with your Firebase API key and database URL.
Replace WIFI_SSID and WIFI_PASSWORD with your Wi-Fi credentials.
Upload the Code: Connect your ESP32 and upload the code.
Code Overview
setup(): Initializes Wi-Fi, Firebase, and sensors.
loop(): Continuously reads sensor data, controls environmental parameters, and writes data to Firebase.
writeData(): Sends data to Firebase, including temperature, humidity, light status, and intruder status.
readData(): Retrieves and processes data from Firebase for remote control.
Usage
Connect ESP32 to Wi-Fi and monitor output on the serial monitor for connectivity and data logs.
Monitor and Control via Firebase: Access Firebase to monitor real-time farm conditions and control devices remotely.
License
This project is open-source and free to use for educational and personal purposes.

