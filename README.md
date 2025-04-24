# Arduino GPS Watch – Phase One: UTM Tracker (Altoids Tin Edition)

## Overview

This project is the first phase of an Arduino-based GPS tracking watch housed in an Altoids tin. The goal is to create a compact, self-contained device that displays **real-time UTM (Universal Transverse Mercator) coordinates** using a GPS module and OLED screen.

Phase One is focused on foundational functionality:
- Acquiring GPS data via UART
- Parsing and converting to UTM format
- Displaying live coordinates on a 0.96" OLED screen

## Hardware Bill of Materials (BOM)

| Component | Description | Link |
|----------|-------------|------|
| **Microcontroller** | [Seeeduino XIAO SAMD21](https://www.amazon.com/Seeeduino-Smallest-Microcontroller-Interfaces-Compatible/dp/B08745JBRP?th=1) - Ultra compact Arduino-compatible board with USB-C and enough IO for GPS and OLED | [Amazon](https://www.amazon.com/Seeeduino-Smallest-Microcontroller-Interfaces-Compatible/dp/B08745JBRP?th=1) |
| **GPS Module** | [GY-NEO6MV2 with Antenna (NEO-6M)](https://www.amazon.com/GY-NEO6MV2-NEO-6M-Control-Antenna-NEO6MV2/dp/B0B49LB18G/) - Standard GPS module with UART output, used for coordinate tracking | [Amazon](https://www.amazon.com/GY-NEO6MV2-NEO-6M-Control-Antenna-NEO6MV2/dp/B0B49LB18G/) |
| **Display** | [0.96" OLED Display (128x64, I2C)](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/) - Monochrome screen for UTM output and basic interface | [Amazon](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/) |

## Scope – Phase One

- ✅ Display current UTM coordinates in real-time
- ✅ Fit entire unit in an Altoids tin (prototype format)
- ⬜ Add physical interface (button for mode switch or refresh)
- ⬜ Power via USB (eventually migrate to battery)
- ⬜ Minimal footprint wiring and breadboarding for prototyping

This is a working prototype to prove the data pipeline from GPS module → Microcontroller → OLED, all while focusing on UTM coordinate formatting.
