# oxyrobot-esp32-hr-spo2-monitor
ESP32 + MAX30102 Heart Rate &amp; SpO₂ Monitor with Telegram Alert – Real-time IoT health monitor using Arduino.
# OxyRobot: ESP32 Heart Rate & SpO₂ Monitor with Telegram Alert 🚑📲

This project uses an **ESP32 DevKit v1** and a **DFRobot MAX30102 Sensor** to monitor **heart rate and blood oxygen (SpO₂)** in real time. If values exceed safe thresholds, the system automatically sends a **Telegram alert**.

## 🧠 Features
- Reads HR and SpO₂ every 4 seconds using MAX30102
- Sends alert via Telegram if:
  - SpO₂ drops below 97%
  - Heart rate is too low/high
- Wi-Fi connectivity
- Invalid readings (e.g. 255) are ignored

## 🛠 Hardware Used
| Component | Details |
|----------|---------|
| ESP32    | DevKit v1 |
| MAX30102 | DFRobot Gravity Sensor (SEN0344) |
| Power    | USB 5V |

## 📡 Wiring (I²C)
| MAX30102 Pin | ESP32 Pin |
|--------------|-----------|
| VCC          | 3.3V      |
| GND          | GND       |
| SDA          | GPIO21    |
| SCL          | GPIO22    |

## 🛠 Libraries Required
- [`DFRobot_BloodOxygen_S`](https://github.com/DFRobot/DFRobot_BloodOxygen_S)
- WiFi (built-in with ESP32)
- HTTPClient (built-in with ESP32)

## 📲 Telegram Bot Setup
1. Create a bot via [@BotFather](https://t.me/BotFather)
2. Copy your bot token
3. Start the bot with `/start`
4. Get your chat ID via: https://api.telegram.org/bot<your_token>/getUpdates

## 📄 Report
Full documentation available in [`oxyrobot.pdf`](./oxyrobot.pdf)

---

## 🔒 Disclaimer
This project is **not a medical device**. It is for **educational/demo purposes** only.
