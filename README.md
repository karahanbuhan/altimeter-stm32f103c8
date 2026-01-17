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

## Video
TODO!

## Bill of Materials (BOM)
| Component | Low Estimate (Target) | High Estimate (Local/Branded) |
| :--- | :--- | :--- |
| **STM32F103C8T6** | $3.50 | $8.00 |
| **BMP280 Sensor** | $1.50 | $5.00 |
| **TM1637 Display** | $1.50 | $4.50 |
| **2x AA Batteries** | $1.00 | $2.00 |
| **Pertinax Board (5x7 cm)** | $0.50 | $2.00 |
| **Consumables (Wire, Solder)**| $1.00 | $4.00 |
| **Total** | **$9.00** | **~$25.50** |

## Schematics
TODO!

## Tests & Calibration
TODO!
