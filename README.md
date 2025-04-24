# GridSnap

## Overview

![Static Badge](https://img.shields.io/badge/version-v1.0-blue)   
This project is the first phase of a lightweight, low-power GPS coordinate viewer for backpackers. ~~Designed to fit inside an Altoids tin and~~ powered by a USB battery bank, the device provides **real-time UTM (Universal Transverse Mercator) coordinates** with no extra features or fluff.    Built by a backpacker, for backpackers. Lightweight, no-nonsense tools for analog-first navigation.

Ideal for those who use traditional map-and-compass navigation and simply want a quick UTM reference without logging, direction, or touchscreen interaction.   

> ⚠️ This project depends on the UTMConversion library by Rafael Reyes Carmona (GPL licensed).
> You must install it manually: [https://github.com/RafaelReyesCarmona/UTMConversion](https://github.com/RafaelReyesCarmona/UTMConversion)

---

## Features

✅ Displays UTM Easting and Northing  
✅ Cold boot checklist on OLED screen  
⬜ Fits inside an Altoids tin (WIP)  
✅ Powered by external USB battery bank  
✅ Zero-button interface, zero configuration  

---

## Bill of Materials (BOM)

| Component | Description | Link |
|----------|-------------|------|
| **Microcontroller** | [Seeeduino XIAO SAMD21](https://www.amazon.com/Seeeduino-Smallest-Microcontroller-Interfaces-Compatible/dp/B08745JBRP?th=1) – Ultra-compact Arduino-compatible board | [Amazon](https://www.amazon.com/Seeeduino-Smallest-Microcontroller-Interfaces-Compatible/dp/B08745JBRP?th=1) |
| **GPS Module** | [GY-NEO6MV2 GPS (NEO-6M)](https://www.amazon.com/GY-NEO6MV2-NEO-6M-Control-Antenna-NEO6MV2/dp/B0B49LB18G/) – Basic UART GPS with patch antenna | [Amazon](https://www.amazon.com/GY-NEO6MV2-NEO-6M-Control-Antenna-NEO6MV2/dp/B0B49LB18G/) |
| **Display** | [0.96" OLED (128x64, I2C)](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/) – Monochrome screen for minimal text display | [Amazon](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/) |

---

## Wiring Diagram

```text
XIAO SAMD21       GPS (NEO-6M)
----------------  ------------
TX (D6)           RX
RX (D7)           TX
GND               GND
3.3V              VCC

XIAO SAMD21       OLED (I2C)
----------------  ------------
D4 (SDA)          SDA
D5 (SCL)          SCL
GND               GND
3.3V              VCC
```
> ⚠️ Ensure your GPS and OLED modules support 3.3V logic (XIAO runs at 3.3V natively).

![Seeeduino XIAO Pinout](docs/xiao-pinout.png)

---

## Intended Use
- Backpacking and wilderness navigation
- Rapid reference UTM coordinate readout
- Plug-and-glance behavior
- Cost effective alternative to commercial options 

> This tool is not designed for real-time navigation, route planning, or speed tracking. It is a modern complement to traditional orienteering.

---

## Changelog   

### V1.0
- Initial version release.

---

## Out-of-Scope for Version One
- ❌ Logging or GPX saving
- ❌ Buttons, menus, or interaction
- ❌ Altitude, heading, or speed
- ❌ Waterproofing / ruggedization
- ❌ Internal battery
- ❌ E-ink screen

---

## Future Versions (Optional Ideas)
- Waypoint memory (5-slot rolling buffer)
- Rechargeable internal battery + solar option
- Custom rugged case or wristband housing
- Sleep mode
- Button-controlled UI for cycling views
- E-ink screen

---

## License
This project is open source under the MIT License.

---

## Author
GridSnap created by KeatonMayhem (C) 2025
