# Smart Parking System 🚗

## Project Description
The Smart Parking System is an IoT-based solution that simplifies parking management in public spaces such as malls. This system leverages ultrasonic sensors, edge devices, and a centralized gateway to monitor and manage parking availability in real-time. The project is designed to provide an efficient and user-friendly experience for both visitors and administrators.

---

## Features  ✨
- **Real-Time Slot Monitoring**:
  - Displays the number of available parking slots.
  - Indicates slot status using green (available) and red (occupied) LEDs.
- **Entry and Exit Detection**:
  - Monitors car movement at entry and exit points.
  - Alerts when the parking lot is full using a buzzer.
- **Firebase Integration**:
  - Real-time data synchronization with Firebase Realtime Database.
  - Stores parking slot status and entry/exit activity.
- **LCD Display**:
  - Displays the number of available slots.
  - Shows "Parking Full" when all slots are occupied.
- **MQTT Communication**:
  - Facilitates communication between edge devices and the gateway.
  - Ensures efficient data transfer.

---

## Architecture 📊
The system consists of the following components:
1. **Edge Device 1 (Slots/Sensors)**:
   - Collects data from ultrasonic sensors for each parking slot.
   - Controls LEDs to indicate slot availability.
   - Sends data to the gateway via MQTT.
2. **Edge Device 2 (Gate)**:
   - Monitors entry and exit points using sensors.
   - Controls a buzzer to alert when slots are full.
   - Sends updates to the gateway via MQTT.
3. **Gateway**:
   - Acts as the central hub for data communication.
   - Updates Firebase with slot availability and gate activity.
   - Fetches slot data for display on the LCD screen.
4. **LCD Display**:
   - Provides real-time updates on parking slot availability.
5. **Firebase Realtime Database**:
   - Stores data for easy access and synchronization.
6. **MQTT Protocol**:
   - Ensures fast and reliable communication between devices.

---

## Setup Instructions 🛠️
### Hardware Requirements
- ESP32 microcontrollers (for gateway and edge devices).
- Ultrasonic sensors for slot monitoring.
- LEDs (green and red) for status indication.
- LCD display for real-time slot updates.

### Software Requirements 🖥️
- Arduino IDE with necessary libraries:
  - `WiFi.h`
  - `Firebase_ESP_Client`
  - `PubSubClient`
- Firebase account for database setup.
- MQTT broker configuration.

### Installation Steps 📝
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/smart-parking-system.git
   ```
2. Open the code files in Arduino IDE.
3. Configure Wi-Fi and Firebase credentials in the gateway and edge device code.
4. Upload the code to ESP32 devices.
5. Set up the hardware components as per the circuit diagram.
6. Power on the devices and monitor the system.

---

## Usage 🔧
- Monitor the LCD display for real-time parking slot availability.
- Access Firebase to review historical data and monitor activity logs.
- Use the buzzer alerts to manage overflows effectively.

---

## Contribution Guidelines  🤝
We welcome contributions to enhance the system! To contribute:
1. Fork the repository.
2. Create a new branch for your feature/bugfix.
3. Commit your changes and push them to your fork.
4. Submit a pull request describing your changes.

---

## License 📄
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---


