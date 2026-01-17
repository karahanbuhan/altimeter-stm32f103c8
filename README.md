# Altimeter STM32F103C8
A simple altimeter built with an STM32F103C8T6 and a BMP280 sensor. The MCU reads pressure data over the I2C bus, calculates the current altitude in meters, and displays the result on a 7-segment (TM1637) display. Includes additional features such as temperature and pressure reading modes.

## Features
- 2 buttons for switching between modes and modify sea level pressure.
- Functional altimeter mode, calculated with pressure difference between manually inputted sea level pressure.
- Temperature (in Celsius) reading mode.
- Pressure (in millibars) reading mode.

## Images
## Altimeter Modes (Meters)
<img width="792" height="1120" alt="Altimeter Mode" src="https://github.com/user-attachments/assets/aaed7c57-f774-40c7-bea2-d800759ac413" />
<img width="940" height="560" alt="Altimeter Mode" src="https://github.com/user-attachments/assets/7fbf1d40-9cf8-4ce8-aaf0-b956fa8a332d" />

## Showing Pressure (Millibars)
<img width="738" height="490" alt="Pressure Mode" src="https://github.com/user-attachments/assets/c18e3038-b07a-4650-b6b2-a2c207a784e9" />

## Showing Temperature (Celsius Degrees)
<img width="732" height="460" alt="image" src="https://github.com/user-attachments/assets/c29c0ebf-fb40-491c-b6b4-a609de4c6b8c" />

### Demo Videos
- **[Project Walkthrough (YouTube)](https://www.youtube.com/watch?v=ruwWq_uKEkQ)** - Detailed explanation of the implementation (in Turkish).
- **[Live Demo (Shorts)](https://www.youtube.com/shorts/OypFeyjmf-c)** - Quick look at the altimeter features in action.

## Bill of Materials (BOM)
| Component | Low Estimate (Target) | High Estimate (Local/Branded) |
| :--- | :--- | :--- |
| **STM32F103C8T6** | $3.50 | $8.00 |
| **BMP280 Sensor** | $1.50 | $5.00 |
| **TM1637 Display** | $1.50 | $4.50 |
| **2x AA Batteries** | $1.00 | $2.00 |
| **Pertinax Board (6x8 cm)** | $0.50 | $2.00 |
| **Consumables (Wire, Solder)**| $1.00 | $4.00 |
| **Total** | **$9.00** | **~$25.50** |

## Schematic
```mermaid
graph TD
    %% MCU
    STM32{STM32}

    %% POWER
    BAT[2x AA Batteries] -- 3V and GND --> STM32

    %% SENSORS
    STM32 -- I2C SCL PB6 SDA PB7 --> BMP[BMP280 Sensor]
    STM32 -- CLK PB5 DIO PB4 --> SCREEN[TM1637 Display]

    %% BUTTON CONNECTIONS
    BTN1[Buton MOD] -- PA4 --> STM32
    BTN2[Buton EDIT] -- PB1 --> STM32

    %% POWER RAIL
    BAT -. VCC GND .-> BMP
    BAT -. VCC GND .-> SCREEN
    BAT -. VCC .-> BTN1
    BAT -. VCC .-> BTN2
```

## Project Status: Archived / Prototype
This project served as a proof-of-concept for the Embedded Systems course, focusing on I2C communication, GPIO manipulation, and system integration on the STM32F103.
### Known Limitations & Retrospective:
**Calibration:** The sensor data is raw. No compensation algorithms or unit tests were implemented.

**Hardware Stability:** The prototype relies on a pertinax (perfboard) circuit. Mechanical instability and signal integrity issues (short/open circuits) occasionally cause the display to freeze, likely due to I2C bus lockups.

**Next Steps:** Future iterations would require a custom PCB design to resolve these mechanical reliability issues.
