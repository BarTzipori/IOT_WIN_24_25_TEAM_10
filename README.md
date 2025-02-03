
![playstore](https://github.com/user-attachments/assets/dfab6fe2-dd94-4fbc-b0e1-ba7d3b094f28)

# SafeStep: Redefining Mobility for the Visually Impaired

## Submitted by Team 10, IOT winter 24-25:
* Name 1
* Name 2
* Name 3

# Short demo:

[![YouTube](http://i.ytimg.com/vi/N4xg39ReWNw/hqdefault.jpg)](https://www.youtube.com/watch?v=N4xg39ReWNw)

# Description:

SafeStep is a compact, innovative system designed to assist visually impaired individuals in detecting obstacles approaching their upper body (chest and head). While traditional aids like walking sticks and guide dogs (K9s) effectively address obstacles at ground level, they often fail to provide protection against upper-body hazards such as low-hanging branches, protruding objects, or other unseen obstructions.

SafeStep bridges this gap by integrating advanced sensors and intuitive alert mechanisms to offer an added layer of safety. The system provides real-time feedback through audio cues, ensuring immediate awareness of potential hazards. For environments where silence is preferred, SafeStep also features vibration alerts, allowing users to choose the mode that best fits their needs.

Lightweight, portable, and user-friendly, SafeStep ensures enhanced awareness and significantly reduces the risk of injury, allowing individuals to navigate their environments with greater confidence and independence. Designed with inclusivity and practicality in mind, SafeStep is the perfect companion for visually impaired individuals seeking a safer and more empowering mobility solution.

## SafeStep: Obstacle Detection Algorithm

SafeStep is equipped with an advanced obstacle detection algorithm that ensures visually impaired users can navigate safely and confidently. The algorithm operates in two primary modes — **Time-to-Impact Mode** and **Distance Mode** — along with a **Degraded Mode** for fallback functionality.

### How It Works:

1. **Detection**:
   - Distance sensors continuously monitor for obstacles in the user’s path.
2. **Processing**:
   - Calculates either the time-to-impact or the proximity to the nearest obstacle.
3. **Alerting**:
   - Issues real-time alerts via audio cues or vibration patterns, depending on user preferences.
---

## Algorithm features:

### **1. Time-to-Impact Mode**
This mode calculates the time before a user reaches an obstacle and provides timely alerts.

- **Obstacle Detection**: 
  - Uses 4 distance-measuring sensors positioned at various angles above the horizon.
  - Calculates both the horizontal (x) and vertical (z) distances from the user’s waist to obstacles.
- **Obstacle Filtering**: 
  - Obstacles above the user’s head (plus a configurable safety margin) or below the selected minimum height are ignored.
- **User Speed Estimation**: 
  - Step frequency is determined using a built-in step counter.
  - Stride length is estimated based on the user’s height, enabling walking speed calculation.
- **Configurable Alerts**: 
  - Alerts are triggered based on the time before impact.
  - Supports up to **3 alert stages**, each with:
    - Custom timing thresholds.
    - Unique vibration patterns.
    - Distinct alert sounds.

---

### **2. Distance Mode**
This mode triggers alerts based on the user’s proximity to obstacles, and not based on the time to impact.

- **Alert by Distance**: 
  - Alerts are issued when obstacles are within user-defined distance thresholds, only while the user moves.
  - Supports up to **3 distance-based alerts**, each customizable with specific sounds and vibration patterns.

---

### **3. Degraded Mode**
Ensures functionality even in the event of sensor malfunctions (e.g., acceleration sensor issues). 
When the acceleration sensor fails, the system will revert to distance mode, regardless of user's choice. Degraded mode, however, 
will disregard speed (without the acceleration sensor, it cannot be calculated effectively), and alert user of obstacles **even if the user is not moving**.

- **Fallback to Distance-Based Alerts**: 
  - Distance-based alerts are triggered regardless of the user’s speed.
  - Ensures users are alerted even if they are not actively moving toward the obstacle.
- **Seamless Transition**: 
  - Automatically switches to degraded mode without interrupting functionality.

---

## Customization and User Control

SafeStep provides a high degree of customizability to meet individual user preferences:

- **Obstacle Filtering**:
  - Configurable safety margins to exclude obstacles above the user’s head.
  - Configurable minimum obstable height, which will dictate the minimum height (above the system) that obstacles will be alerted for.
- **Alert Configuration**:
  - Customizable thresholds for time-to-impact and distance-based alerts.
  - Personalizable vibration patterns and alert sounds for each stage.
- **Flexibility**:
  - Users can toggle between modes based on their needs and environment.

---


## System Redundancy

The algorithm is designed with reliability in mind, ensuring safety even during sensor malfunctions:

- **Degraded Mode Activation**:
  - Automatically engages when speed calculations are unavailable.
- **Consistent Alerts**:
  - Alerts are based on distance alone, maintaining user awareness.

---

### Degraded modes:

SafeStep degraded modes are designed to ensure the system's ability to function wven when some components stop working. 

<img width="1126" alt="SafeStep_sensors_degraded_modes" src="https://github.com/user-attachments/assets/b5aff7b9-d6dc-4070-b9b6-dbc230b0bb45" />

---

## System Settings - saving, loading and updating:

SafeStep offers a flexible and user-friendly system for managing settings, ensuring the device can adapt to individual preferences and operate seamlessly in various environments.

### **1. On-Board Memory Storage**
- SafeStep saves all settings on an **on-board SD card** for persistent storage.
- This ensures that user preferences and configurations are retained even if the system is powered off or restarted.

### **2. Automatic Updates via Wi-Fi**
- If the system detects an active Wi-Fi connection during start up:
  - It will connect to the SafeStep **central database** to check for updated settings or configurations.
  - If new settings are available, the system will download and apply them automatically.
  - The updated settings will override the existing configuration on the SD card, ensuring the latest adjustments are always applied.
- This feature allows for remote updates and synchronization with user preferences saved in the SafeStep app.

### **3. Adjusting Settings with the SafeStep App**
- Users can customize system settings easily through the **SafeStep mobile app**:
  - Modify alert thresholds (time-to-impact or distance).
  - Configure vibration patterns and sound types for alerts.
  - Set personal preferences, including safety margins and operating modes.
- Once settings are updated in the app, they are automatically synced with the device via Wi-Fi or Bluetooth.
- The app also provides a user-friendly interface for monitoring system status and accessing additional features.

The following chart describes the process presented above:

<img width="1169" alt="safestep_settings_flowchart" src="https://github.com/user-attachments/assets/a58e31e7-946e-4dde-81f2-ad68c677e672" />

---

## System Logs

SafeStep includes a comprehensive logging system that records key events and activities to improve performance monitoring, troubleshooting, and user safety. These logs provide valuable insights into system behavior and can be used for diagnostics or analysis.

### **Logging Features**

1. **Event Documentation**
   - The system automatically records events in a log file stored on the SD card.
   - Logged events include:
     - Obstacle detections.
     - Button presses.
     - Mode changes.
     - System status updates.
   - This ensures a clear history of interactions and detections for future reference.

2. **Obstacle Image Capture (Optional)**
   - If enabled via the **SafeStep app**, the onboard camera will capture an image of every detected obstacle.
   - These images are stored alongside the log entries, providing a visual record of obstacles encountered.

3. **Log Upload via Wi-Fi**
   - When the system is powered on and a Wi-Fi network is available, the user will be prompted with an option to upload logs and images to the **SafeStep database**.
   - To initiate the upload, the user simply needs to **press the action button once**.
   - This allows users to back up their logs and images for later review or technical support.

4. **System Error Reporting**
   - At any time, the user can manually report a system error by pressing the **Action Button (medium press, 3-4 seconds)**.
   - This will:
     - Document the error in the log file.
     - Capture an image of the environment using the onboard camera for further analysis.
   - This feature helps in troubleshooting and diagnosing unexpected system behavior.
  
     <img width="1626" alt="Screenshot 2025-02-02 at 21 21 21" src="https://github.com/user-attachments/assets/1e39f7cb-be42-4ec4-850d-0d71286fbead" />


---

## Action Button:

The SafeStep system includes a versatile **Action Button** that allows users to quickly access key features without needing to use the mobile app. This button provides intuitive control for critical functions, ensuring ease of use in various scenarios.

### **Button Functions**

1. **Short Press**:  
   - Toggles **silent mode** on and off.
   - In silent mode, the system switches from audio cues to vibration-only alerts, offering a quieter experience.

2. **Medium Press (2-4 seconds)**:  
   - **Reports a system error**:
     - Logs the error in the system's onboard log file for future diagnostics.
     - Activates the onboard camera to capture an image of the environment in front of the system at the time the error occurred.
   - This feature helps users or support teams understand and resolve issues effectively.

3. **Double Press**:  
   - Initiates **pairing mode**:
     - Enables the system to connect to a new Wi-Fi network.
     - The system enters a discoverable state, allowing the user to easily pair it with the desired network via the SafeStep app.

4. **Long Press (More than 10 seconds)**:  
   - Launches the **MPU (Motion Processing Unit) calibration routine**:
     - Resets and recalibrates the system’s motion sensors to ensure optimal performance.
     - Useful for troubleshooting or after significant environmental or operational changes.

---

# SafeStep App  

The **SafeStep App** is the official companion application for customizing and managing your SafeStep system. It provides an intuitive interface for adjusting settings, and ensuring a personalized experience.  

## Features  

### **Customization & Alerts**  
- Configure **Time-to-Impact** and **Distance-Based Alerts**  
- Adjust **vibration patterns** and **sound types** for different alert stages
- Change alerts volume
- Enable or disable up to three alerts
    
- Set **safety margins** to filter out unnecessary obstacle detections  
  
The SafeStep App makes it easy to fine-tune our device, ensuring a smooth and reliable user experience.   

![Screenshot_20250203-154627](https://github.com/user-attachments/assets/e35d9cc5-ce31-438a-945e-60007addec3f)
![Screenshot_20250203-154508](https://github.com/user-attachments/assets/b18b4e4e-d8ab-4973-ad4c-19baeb1ff87d)
![Screenshot_20250203-154728](https://github.com/user-attachments/assets/1b8a9979-54cf-4c83-97c5-bc9f46b6e835)
![Screenshot_20250203-154643](https://github.com/user-attachments/assets/8d8175e2-ffe6-411d-8c98-e1ef86338d5a)
![Screenshot_20250203-154701](https://github.com/user-attachments/assets/e61a9ee1-cc8c-4eba-8111-146f12a0fbfe)


## Wiring diagram :

![safeStep_wiring_diagram](https://github.com/user-attachments/assets/57ba782f-c5d0-4710-b2f8-c96a68f1d6c3)

---

## System components (hardware) :

1. X4 VL53L1X distance measuring sensors (TOF)
2. X1 MPU9250
3. X1 Serial mp3 player
4. X2 Vibration motors
5. X1 Push button
6. X1 Power bank


![4CC7AF4D-2CD7-439F-9544-25C52A1B7D84_1_105_c](https://github.com/user-attachments/assets/079f2cbf-9345-4801-877a-231146137571)

![9567E2B6-2CAC-4E3A-943B-4917120BA23E_1_105_c](https://github.com/user-attachments/assets/944df19a-c4d3-41a3-8381-7929e12f7fd2)

![47E96DC5-61A4-4CBD-8839-C6EB57CD4383_1_105_c](https://github.com/user-attachments/assets/8c64f425-6851-4f5a-9700-551c05cf996f)

---


## Folder description :
* ESP32: source code for the esp side (firmware).
* Src: source code for platform io (cpp files)
* Include: source code for platform io compilation (header files)
* Documentation: wiring diagram + basic operating instructions
* Unit Tests: tests for individual hardware components (input / output devices)
* flutter_app : dart code for our Flutter app. 
* Assets: 3D printed parts, Audio files used in this project.

## Arduino/ESP32 libraries used in this project:

* 	adafruit/Adafruit VL53L1X@^3.1.2
* 	adafruit/Adafruit BusIO@^1.16.2
* 	espressif/esp32-camera@^2.0.4
* 	plerup/EspSoftwareSerial@^8.2.0
* 	SPI
* 	Wire
* 	adafruit/Adafruit Unified Sensor@^1.1.14
* 	arduinogetstarted/ezButton@^1.0.6
* 	hideakitai/MPU9250@^0.4.8
* 	arduino-libraries/ArduinoMqttClient@^0.1.8
* 	mobizt/Firebase Arduino Client Library for ESP8266 and ESP32@^4.4.17
* 	tzapu/WiFiManager@^2.0.17
* 	 phome/AsyncTCP-esphome@^2.1.4
* 	 esphome/ESPAsyncWebServer-esphome@^3.3.0
* 	 gilmaimon/ArduinoWebsockets@^0.5.4
* 	 ayushsharma82/WebSerial@^2.0.8

## Project Poster:
 
This project is part of ICST - The Interdisciplinary Center for Smart Technologies, Taub Faculty of Computer Science, Technion
https://icst.cs.technion.ac.il/
