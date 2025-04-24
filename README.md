# Arduino GPS Tracker – Phase One: UTM Tracker (Altoids Tin Edition)

## Overview

This is Phase One of a compact Arduino-based GPS watch project, designed to display **real-time UTM coordinates** on a small OLED screen. It is designed to fit inside an Altoids tin for prototyping and portability.

## Features

✅ Acquires GPS signal via NEO-6M  
✅ Converts coordinates to UTM format  
✅ Displays Easting (X) and Northing (Y) in large font  
✅ Displays local time and date from GPS data  
⬜ Future features include waypoints, button interaction, and logging

## Bill of Materials (BOM)

| Component | Description | Link |
|----------|-------------|------|
| **Microcontroller** | [Seeeduino XIAO SAMD21](https://www.amazon.com/Seeeduino-Smallest-Microcontroller-Interfaces-Compatible/dp/B08745JBRP?th=1) – Compact SAMD21 board with USB-C | [Amazon](https://www.amazon.com/Seeeduino-Smallest-Microcontroller-Interfaces-Compatible/dp/B08745JBRP?th=1) |
| **GPS Module** | [GY-NEO6MV2 GPS (NEO-6M)](https://www.amazon.com/GY-NEO6MV2-NEO-6M-Control-Antenna-NEO6MV2/dp/B0B49LB18G/) – Serial GPS module with antenna | [Amazon](https://www.amazon.com/GY-NEO6MV2-NEO-6M-Control-Antenna-NEO6MV2/dp/B0B49LB18G/) |
| **Display** | [0.96" OLED Display (I2C, 128x64)](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/) – For displaying UTM coordinates | [Amazon](https://www.amazon.com/Hosyond-Display-Self-Luminous-Compatible-Raspberry/dp/B09T6SJBV5/) |

## Wiring

```text
XIAO SAMD21       GPS (NEO-6M)
----------------  ------------
TX (D6)           RX
RX (D7)           TX
GND               GND
3.3V              VCC

XIAO SAMD21       OLED Display (I2C)
----------------  ------------------
D4 (SDA)          SDA
D5 (SCL)          SCL
GND               GND
3.3V              VCC
