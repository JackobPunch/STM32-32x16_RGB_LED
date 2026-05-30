# STM32 32x16 RGB LED Matrix Countdown Timer

![Countdown Timer Demo](gif.gif)

_Sped-up 4x demo showing all timer states on dual displays_

STM32-driven countdown timer for a 32×16 RGB LED matrix — built for the Smash Bot Arena at ROBOCOMP 2025, KN Integra's robotics competition at AGH University. Four units ran on the arena sides during the event.

One of two modules in the system: see also [STM32_MODBUS_RTU_slave_sensor_module](https://github.com/JackobPunch/STM32_MODBUS_RTU_slave_sensor_module).

## Hardware

| | |
|---|---|
| MCU | STM32F413ZHT6U (Cortex-M4, 100 MHz) |
| Display | 32×16 RGB LED matrix, HUB75, 1/8 scan |

## Pin Configuration

| Pin | Function |
|-----|----------|
| PA5–PA7 | R1, G1, B1 (upper half) |
| PA8–PA10 | R2, G2, B2 (lower half) |
| PB3–PB5 | A, B, C (row select) |
| PB6 | D (half select) |
| PB10–PB12 | CLK, LAT, OE |
| PC6, PC8, PC9 | STOP, RESET, START |

## Visual States

| State | Label | Color |
|-------|-------|-------|
| Ready | READY / ST8DY alternating | Yellow + Blue 03:00 |
| Running | GO!!! | Green + Red countdown |
| Stopped | STOP | Red + Yellow countdown |
| Finished | STOP | Red + Yellow 00:00 |

## Dependencies

- [Adafruit RGB Matrix Panel](https://github.com/adafruit/RGB-matrix-Panel) — adapted for STM32 HAL
- STM32 HAL, ARM GCC, STM32CubeIDE
