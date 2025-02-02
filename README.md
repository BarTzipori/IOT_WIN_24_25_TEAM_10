
![playstore](https://github.com/user-attachments/assets/dfab6fe2-dd94-4fbc-b0e1-ba7d3b094f28)

## SafeStep - impaired vision device: 

## Submitted by Team 10, IOT winter 24-25:
* Name 1
* Name 2
* Name 3  

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/N4xg39ReWNw/0.jpg)](https://www.youtube.com/watch?v=N4xg39ReWNw)
  
## SafeStep: Redefining Mobility for the Visually Impaired

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

## Folder description :
* ESP32: source code for the esp side (firmware).
* Documentation: wiring diagram + basic operating instructions
* Unit Tests: tests for individual hardware components (input / output devices)
* flutter_app : dart code for our Flutter app.
* Parameters: contains description of configurable parameters 
* Assets: 3D printed parts, Audio files used in this project, 

## Arduino/ESP32 libraries used in this project:
## Arduino/ESP libraries installed for the project:
* XXXX - version XXXXX
* XXXX - version XXXXX
* XXXX - version XXXXX

## Project Poster:
 
This project is part of ICST - The Interdisciplinary Center for Smart Technologies, Taub Faculty of Computer Science, Technion
https://icst.cs.technion.ac.il/
