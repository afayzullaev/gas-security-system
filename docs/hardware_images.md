# Hardware Design Images

This document provides descriptions of the hardware design images included in the project.

## PCB Design

The PCB design created in EasyEDA (`pcb_easyeda.png`) shows the complete layout with component placement and routing. The design includes:

- ESP32 microcontroller placement in the center
- ADS111x ADC component for sensor readings
- SSD1306 OLED display connector
- SIM800 GSM modem integration
- Gas sensor connectors for CH4 and CO
- Power circuitry
- Status LED and buzzer connections

## PCB Implementation

The manufactured PCB images show the final implementation:

- `pcb_front.jpg`: Front view of the assembled PCB showing the ESP32, display connector, and sensor connections
- `pcb_back.jpg`: Back view with the SIM800 modem, power management circuitry, and additional components

## Installation

The `Installation.jpg` image shows a proper installation of the gas detection system in a residential setting. Key aspects shown:

- Proper mounting height for optimal gas detection
- Connection to gas valve control system
- Power supply integration
- Status indicators visible to residents

## Schematic

The `schematic.pdf` contains the complete electrical schematic with:

- Detailed pin connections for all components
- Power distribution system
- Communication interfaces (I2C, UART)
- Sensor integration details
- Input/Output connections

These design files are available in the main directory of the source code repository.