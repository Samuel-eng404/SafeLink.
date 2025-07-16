# SafeLink – ESP32-Based Proximity Bracelet System

SafeLink is a graduation project developed to establish a **wireless proximity link** between parents and their children using the **D1 Mini ESP32**. The system ensures real-time monitoring, directional guidance, and safety alerts through a wearable device, helping prevent child abduction or unexpected separation.

## 🛠️ Project Overview

- **Platform:** ESP32 (D1 Mini)
- **Parent Bracelet Features:**
  - Wi-Fi communication (AP + STA modes)
  - Real-time distance estimation using RSSI
  - Compass-based directional arrow pointing toward the child
  - TFT display (showing time, battery, distance, status, and icons)
  - Buzzer and vibration motor for alerts
  - Reed switch for tamper detection
- **Child Bracelet Features (separate repo or code):**
  - Sends RSSI + battery + tamper status to the parent bracelet
  - Detects SOS button press
  - Secured with a reed switch

## 💡 Key Features

- 📡 **Wi-Fi Communication:** ESP32 in dual mode — acts as AP for the child and connects as STA to parent’s phone.
- 🧭 **Compass-Based Direction:** The parent bracelet uses the QMC5883L sensor to display the direction to the child.
- 📏 **Distance Estimation:** RSSI values are used to approximate how far the child is.
- ⏰ **NTP Time Sync:** Parent bracelet synchronizes real-time clock from internet via NTP.
- 🔋 **Battery Monitoring:** Battery percentages shown for both devices.
- ⚠️ **Alerts:** Sound, vibration, and on-screen warnings for:
  - Distance thresholds
  - Tampering (reed switch)
  - Disconnection or timeout
 
## 🧰 Libraries Used

- WiFi.h
- WiFiUdp.h
- NTPClient.h
- time.h
- Wire.h
- MechaQMC5883.h
- TFT_eSPI.h

## 📷 Screenshots / Media
<div align="center">
  <img src="https://github.com/user-attachments/assets/ad7c615a-64eb-40ef-98b1-5f8ace6e6ffd" width="350" height="350" alt="Screenshot 1" style="margin-right: 20px;" />
  <img src="https://github.com/user-attachments/assets/06812145-f4ab-43a8-8584-94b006f13d7e" width="350" height="350" alt="Screenshot 2" />
</div>

## 📦 Hardware Components

| Component            | Description                          |
|----------------------|--------------------------------------|
| D1 Mini ESP32        | Wi-Fi microcontroller                |
| QMC5883L Compass     | For directional guidance             |
| TFT LCD (gc9a01)     | Display info on the parent bracelet  |
| Buzzer & Vibration   | For alerts                           |
| Reed Switch          | Detect bracelet removal (child side) |
| Battery & Charger    | Power source                         |

## 🧪 How It Works

```mermaid
graph TD;
    Child[Child Bracelet] -->|Wi-Fi RSSI, Battery, Reed| Parent[Parent Bracelet]
    Parent -->|Display: Arrow, Time, Alerts| TFT[TFT Screen]
    Parent -->|Sound + Vibration| Alerts[Alert System]

