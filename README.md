# Remote Control using MQTT & Django

This project involves the development of a **Django-based web server** running on a **Raspberry Pi**, enabling remote control of an **ESP32** via **MQTT** protocol. The system allows for controlling relays and fetching temperature data via a web interface or **RFID** authentication. Remote access is secured using **SSH** with RSA keys, and **VNC** is used for graphical interface access.

## Project Overview

- **Raspberry Pi** serves as the central server, hosting the Django application.
- **ESP32** communicates with the Django server via MQTT and sends temperature data.
- The web interface allows control of relays (e.g., for switching lights or other devices) and the ability to read temperature from the sensor.
- **RFID** module is used for user authentication, enabling access control.
- MQTT is used for efficient real-time communication between the ESP32 and Django server.
- The system ensures **secure remote access** using SSH (port forwarding, RSA keys) and **VNC** for remote graphical interface control.

## Key Features

- **Django-based Web Server**: Developed using Python’s Django framework to create a user-friendly web interface for relay control and sensor monitoring.
- **MQTT Communication**: Utilized the **Paho MQTT** library to establish a local MQTT broker for communication between the Raspberry Pi and the ESP32.
- **Temperature Monitoring**: The system reads and displays temperature data randomly generated using a C library on ESP32
- **Relay Control**: Allows switching relays remotely through the web interface or RFID authentication.
- **Secure Remote Access**: Configured **SSH** (with RSA keys) for secure terminal access and **VNC** for remote desktop control.
- **RFID Authentication**: Secures access to relays based on authorized RFID cards.

## Requirements

- **Raspberry Pi** with Raspberry Pi OS installed.
- **ESP32** board.
- **Django** (for web server setup).
- **Paho MQTT** (for MQTT broker and communication).
- **RFID RC522** module.
- **DHT11** sensor (for temperature data).
- **VNC** server for remote desktop access.
- **SSH** with RSA key setup for secure connection.

## Future Improvements

1. **User Authentication and Authorization**:
   - Implement a user authentication system on the web interface to ensure that only authorized users can access relay control and sensor data.

2. **Support for Additional Sensors**:
   - Extend the system to support additional sensors (e.g., humidity sensor, light sensor) and display this data in real-time on the web page.

3. **Cloud Integration**:
   - Integrate the system with cloud platforms (e.g., AWS, Google Cloud, or Azure) to store and access data remotely for monitoring and control.

4. **Push Notifications**:
   - Add push notification functionality to alert users when the temperature exceeds a certain threshold or when there are significant changes in the system (e.g., price drop alerts based on keywords).

5. **Automated Relay Control**:
   - Implement automation rules for relay control based on incoming data (e.g., automatically turning on lights when the light sensor detects low illumination).

6. **Mobile App Development**:
   - Develop a mobile app (for Android or iOS) to control the devices remotely using MQTT, offering a more user-friendly interface and faster access.

7. **Enhanced Security**:
   - Implement data encryption for MQTT communication (using TLS/SSL) and additional security measures to protect communications and access to the system.

8. **Smart Home Integration**:
   - Enable integration with smart home platforms such as **Home Assistant** or **OpenHAB**, allowing IoT devices to be controlled within a smart home environment.

9. **Advanced Dashboard**:
   - Create an advanced dashboard with graphs and detailed statistics on the temperature history and relay status for better monitoring and visualization.

10. **Performance Optimization**:
   - Optimize the system to reduce latency and improve performance, especially for real-time data updates and relay control responsiveness.
