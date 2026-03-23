# 🏛️ LuxGuard — Smart IoT Security System for Luxury Exhibits

**Built by Bijay Mandal** | Aspiring Product Manager

[![Made with ESP32](https://img.shields.io/badge/Hardware-ESP32--CAM-blue)](https://github.com/)
[![Telegram Alerts](https://img.shields.io/badge/Alerts-Telegram%20API-29A8E0)](https://core.telegram.org/bots/api)
[![Status](https://img.shields.io/badge/Status-Prototype%20Complete-brightgreen)](https://github.com/)

---

## 📌 Why This Project Exists

> "Museums spend thousands on security systems that are either too complex to maintain or too simple to trust. We asked: what if a ₹2,000 device could do more than what a ₹1,00,000 system does?"

LuxGuard is a working IoT prototype I co-built with my team to solve a real problem — protecting high-value items in spaces where professional security systems are unaffordable or inaccessible. It targets **small museums, boutique stores, personal collections, and exhibition spaces.**

This project was my deliberate attempt to move beyond theory and understand how products are actually built — the decisions, trade-offs, and complexity that only become visible when you're hands-on.

---

## 🎯 Problem Statement

| Problem | Impact |
|---|---|
| Professional security systems cost ₹50,000–₹5,00,000+ | Unaffordable for small museums, exhibitions, and independent retailers |
| Generic CCTV doesn't trigger on item displacement | Security teams can't react in real time; incidents are discovered after the fact |
| No immediate alert to the right person | A camera recording is only useful after the crime, not during it |

**LuxGuard's Solution:** A sub-₹2,000 smart device that detects item displacement, warns approaching visitors, and sends a live photo of the incident directly to the owner's phone — all within seconds.

---

## 👤 Target Users

| User | Need |
|---|---|
| **Museum security head** | Get notified immediately with a photo when an exhibit is touched or removed |
| **Boutique / jewelry store owner** | Protect display items without hiring additional staff |
| **Exhibition organizer** | Ensure visitors maintain safe distance from valuable artifacts |
| **Private collector** | Affordable always-on protection for home displays |

---

## ✅ Core Features (v1.0 — Shipped)

| Feature | Description |
|---|---|
| 📏 **Proximity Warning** | Warns visitors with beeps when they come within 50 cm of an item |
| 🚨 **Displacement Alarm** | Continuous alarm triggers if item is moved from its designated position |
| 📸 **Instant Photo Alert** | Camera captures image at the moment of displacement and sends to owner via Telegram |
| 📱 **Real-time Telegram Notification** | Security head receives photo + alert on their phone within seconds — no app install needed |
| 🔄 **Auto-reset** | System resets alarm when item is returned to position |

---

## 🗺️ Product Roadmap

### ✅ v1.0 — Current (Prototype)
- Dual ultrasonic sensor detection
- Buzzer-based warning and alarm
- ESP32-CAM photo capture
- Telegram photo alert to single recipient

### 🔜 v1.1 — Next Sprint
- **Product label in alert** — e.g., *"⚠️ Alert: 'Diamond Necklace — Case 3B' removed at 14:32"*
- Multi-recipient Telegram alerts (alert entire security team, not just one person)
- Configurable distance thresholds without reflashing code

### 🔮 v2.0 — Future Vision
- Web dashboard for alert history and device status
- Battery-powered version for wireless deployment
- Multi-device network — one dashboard, many exhibits
- AI-assisted face crop from captured image for easier identification

---

## 🔩 What We Built With

| Component | Role | Approx. Cost |
|---|---|---|
| ESP32-CAM AI Thinker | Brain + camera of the system | ₹450 - ₹800 |
| HC-SR04 Ultrasonic Sensor × 2 | Item position + person proximity detection | ₹80 |
| Buzzer | Warning beeps + alarm | ₹20 -₹60 |
| FTDI Module | Flash firmware via USB | ₹150 |
| USB Cable, jumper wires | Power + connections | ₹50 - 150 |
| Cardboard + plastic pipe | Enclosure and sensor mounting | ₹30 - 300 |
| **Total** | | **~₹780–₹1,540** | 
>Budget ₹2,000 

---

## ⚙️ How It Works — Technical Overview

```
[HC-SR04 Sensor 1]  →  Is the item still in place?
[HC-SR04 Sensor 2]  →  Is someone too close?
        ↓
   [ESP32-CAM]  →  Process logic, trigger buzzer, capture photo
        ↓
 [Telegram API]  →  Send photo alert to security head's phone
```

**Three operating states:**

| State | Trigger | Response |
|---|---|---|
| 🟢 Safe | Item in place, person at safe distance | Silent — no action |
| 🟡 Warning | Person within 10–50 cm | Short beep pattern (5 beeps) |
| 🔴 Alert | Item displaced (sensor reads > threshold) | Continuous alarm + photo sent to Telegram |

---

## 🧠 What I Learned as a Aspiring PM — The Honest Debrief

This is the section I'd want every hiring manager to read. Building this end-to-end changed how I think about product work.

---

### 1. "Simple features" have invisible complexity

Adding the Telegram photo alert sounds like a one-liner: *"when alarm triggers, send photo."*
The reality involved: HTTPS client setup on a microcontroller, multipart form-data encoding, memory management for image buffers, error handling for connection failures, and camera frame buffer management.

**What I'd do differently as a PM:** I now write acceptance criteria that include *failure states*, not just success states. "User receives photo" also means specifying: *what happens if WiFi drops at trigger time? What if the camera fails to initialize?*

---

### 2. Hardware bugs look like software bugs

We spent hours debugging why alerts weren't sending — it turned out to be a power supply issue causing the ESP32 to brown-out when the camera and WiFi were active simultaneously. Nothing in the code was wrong.

**PM Takeaway:** In IoT products, always allocate separate debugging time for hardware-software interaction. A developer estimating "2 days to integrate the camera" may not be accounting for power, interference, or sensor calibration time.

---

### 3. Async network calls need a retry strategy — always

If WiFi drops at the exact moment the alarm triggers, the photo alert is silently lost. No error. No retry. No log. The owner never knows.

**What I added to our v1.1 spec:** A local queue that stores missed alerts and retries them when connectivity is restored. This is a tiny feature on a spec sheet but critical for product trust.

---

### 4. Real timelines look different from estimated timelines

| Task | My Initial Estimate | Actual Time |
|---|---|---|
| Coding | 6 hours | 10 hours |
| Wire up sensors | 1 hour | 1 hour ✅ |
| Get sensors reading correctly | 30 min | 2 hours (calibration, noise filtering) |
| Camera initialization | 1 hour | 4 hours (power issues, driver config) |
| Telegram API integration | 2 hours | 5 hours (HTTPS, multipart body, memory) |
| Product design & development | 4 hours | 7 hours |
| Testing & edge cases | 2 hour | 4 hours |

**The pattern:** Anything touching external APIs, hardware drivers, or network reliability takes 3–4x longer than estimated. I now build this multiplier into specs automatically.

---

## 🚀 Getting Started

### Prerequisites
- Arduino IDE with ESP32 board package
- Telegram bot created via [@BotFather](https://t.me/BotFather)
- Your Telegram Chat ID from [@userinfobot](https://t.me/userinfobot)

### Setup in 4 Steps

**1. Clone the repo**
```bash
git clone https://github.com/bijaymandal01/Smart-IoT-Security-System-for-Luxury-Exhibits.git
```

**2. Add your credentials** in `main.ino`
```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
String token   = "YOUR_TELEGRAM_BOT_TOKEN";
String chat_id = "YOUR_TELEGRAM_CHAT_ID";
```

**3. Flash to ESP32-CAM**
- Connect via FTDI module
- Board: `AI Thinker ESP32-CAM` | Baud: `115200`
- Upload → Open Serial Monitor to confirm WiFi connection

**4. Deploy**
- Mount sensors on your exhibit stand
- Power via USB or 5V adapter
- System is live and monitoring

> ⚠️ **Before pushing to GitHub:** Move credentials to a `config.h` file and add it to `.gitignore`. Never commit real tokens to a public repo.

---

## 👥MY Role

| **Bijay Mandal** | Product vision, system design, documentation, PM perspective |


*Built to understand what it actually takes to ship a product — not just to write a spec about one.*

**— Bijay Mandal**
