# Arduino Parking System

## General Description
This project is a Smart Parking System that automates parking lot management using IoT components. 
The system detects vehicle presence, verifies authorized access via NFC, controls gate operations, and monitors parking space occupancy. 
It provides real-time feedback through an LCD display and ensures only authorized vehicles can enter when spaces are available.


## Bill of Materials
| Component             | Quantity  | Purpose                      |
|-----------------------|-----------|------------------------------|
| Arduino Uno           |     1     | Main Controller              |
| 16x2 LCD              |     1     | Display status and messages  |
| EEPROM                |           | Saving the cars in park      | 
| Servo Motor           |     1     | Gate arm control             |
| Infrared sensor       |     1     | Entry detection              |
| Ultrasonic sensor     |     1     | Exit detection               |
| Breadboard & Wires    |           | Connections                  |
| Power Supply          |     1     | System Power                 |


## Implementetion Steps and Technologies
### 1. System Architecture Design
- The parking system follows a modular architecture with clear separation of concerns:
- Entry Detection Module: Infrared sensor detects approaching vehicles
- Exit Detection Module: Ultrasonic sensor measures distance to detect departing vehicles
- Gate Control Module: Servo motor with precise angle positioning
- Display Module: 16x2 LCD for real-time status updates
- Storage Module: EEPROM for persistent data storage
- Main Controller: Arduino Uno coordinating all modules


### 2. Software Architecture
Core Libraries:
- Servo.h: Precise servo motor control with angle mapping
- LiquidCrystal.h: LCD display interface with custom messages
- EEPROM.h: Persistent storage for car count data

Main Program Flow:
- Initialization: Load car count from EEPROM, initialize displays and sensors
- Entry Processing: IR sensor debouncing and parking capacity checking
- Exit Processing: Ultrasonic distance measurement and validation
- Gate Management: Timed gate operations with servo control
- Display Updates: Real-time LCD status updates


### 3. Advanced Features Implementation
Anti-Glitch Measures:
- Sensor cooldown periods prevent multiple detections
- State flags prevent race conditions
- Input validation for EEPROM data

Error Handling:
- EEPROM data validation on startup
- Ultrasonic timeout handling (25ms timeout)
- Invalid state recovery mechanisms

Performance Optimizations:
- Non-blocking timing using millis()
- Conditional display updates to reduce LCD operations
- Efficient EEPROM writes to prevent wear

### Pictures of the Setup
  <p align="center">
    <img src="https://github.com/user-attachments/assets/732b7f84-15eb-40c3-b815-0d3ad5399dd0" width="200">
    <img src="https://github.com/user-attachments/assets/e69d23bc-9950-4f7e-8764-c5ef748c08a3" width="200">
    <img src="https://github.com/user-attachments/assets/8e0c2689-a58e-420a-b567-424ed1591301" width="200">
  </p>

### Video of the Working Product
https://youtube.com/shorts/vGLxzjhtAnc
