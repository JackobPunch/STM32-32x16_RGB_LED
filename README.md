# STM32 32x16 RGB LED Matrix Countdown Timer

A complete STM32-based countdown timer display for 32x16 RGB LED matrices using HUB75 interface.

## 🎯 Features

- **Countdown Timer**: Counts down from 03:00 to 00:00 with MM:SS format
- **Persistent Display**: Timer stays visible as "00:00" when countdown finishes
- **Dual Display**: Green "TIMER" label at top, red countdown at bottom
- **8-Color Support**: Full RGB color palette with easy color selection
- **Smooth Updates**: 1-second intervals with no flickering

## 🔧 Hardware

### Board

- **Microcontroller**: STM32F413ZHT6U (ARM Cortex-M4)
- **Clock Speed**: 100MHz
- **Flash Memory**: 1.5MB
- **RAM**: 320KB

### Display

- **Type**: 32×16 RGB LED Matrix Panel
- **Interface**: HUB75 (16-pin)
- **Scan Rate**: 1/8 scan multiplexing
- **Colors**: 8 colors (1-bit RGB per pixel)

## 📌 Pin Configuration

| Pin  | Function | Description             |
| ---- | -------- | ----------------------- |
| PA5  | R1       | Red data (upper half)   |
| PA6  | G1       | Green data (upper half) |
| PA7  | B1       | Blue data (upper half)  |
| PA8  | R2       | Red data (lower half)   |
| PA9  | G2       | Green data (lower half) |
| PA10 | B2       | Blue data (lower half)  |
| PB3  | A        | Row select bit 0 (LSB)  |
| PB4  | B        | Row select bit 1        |
| PB5  | C        | Row select bit 2 (MSB)  |
| PB6  | D        | Half select (always 0)  |
| PB10 | CLK      | Data clock              |
| PB11 | LAT      | Latch signal            |
| PB12 | OE       | Output enable           |

## 🎨 Color System

The display supports 8 colors using 1-bit per RGB channel:

```cpp
// Predefined colors
#define BLACK   0x0000
#define RED     0xF800  // 1,0,0
#define GREEN   0x07E0  // 0,1,0
#define BLUE    0x001F  // 0,0,1
#define YELLOW  0xFFE0  // 1,1,0
#define MAGENTA 0xF81F  // 1,0,1
#define CYAN    0x07FF  // 0,1,1
#define WHITE   0xFFFF  // 1,1,1

// Or use color functions
matrix.setTextColor(Color333(7, 0, 0));  // Red
matrix.setTextColor(Color1bit(1, 0, 1)); // Magenta
```

## 🏗️ Architecture

### Core Components

- **RGBmatrixPanel_STM32**: Custom LED matrix driver class
- **Countdown Logic**: Timer state management and display updates
- **HAL Integration**: STM32 HAL GPIO for direct pin control

### Display Layout

```
Row 0-7:   Green "TIMER" label (left-aligned)
Row 8-15:  Red countdown "MM:SS" (centered)
```

## 📚 Dependencies

### Based On

- **[Adafruit RGB Matrix Panel](https://github.com/adafruit/RGB-matrix-Panel)**: Arduino library for RGB LED matrices - adapted for STM32 HAL
- **Adafruit GFX**: Text rendering and graphics primitives
- **Custom HUB75 Driver**: Adapted for STM32 HAL and 32x16 panels
- **STM32 HAL Library**: GPIO and timing functions

### Development Tools

- **STM32CubeIDE**: Project creation and debugging
- **ARM GCC**: Cross-compilation toolchain
- **OpenOCD**: Programming and debugging interface

## 🚀 Getting Started

### Prerequisites

- STM32CubeIDE 1.13+
- ARM GCC toolchain
- ST-Link programmer
- 32×16 RGB LED matrix panel
- STM32F413ZHT6U board

### Building

1. Open project in STM32CubeIDE
2. Select "Debug" configuration
3. Build project (Ctrl+B)
4. Program to board

### Hardware Setup

1. Connect LED matrix to HUB75 interface pins
2. Power the matrix (5V, 2A recommended)
3. Program STM32 board
4. Countdown starts automatically on power-up

## 📊 Performance

- **Update Rate**: 1 Hz (1-second intervals)
- **Binary Size**: ~14.6KB
- **RAM Usage**: ~2KB
- **Power Consumption**: ~150mA (matrix dependent)

## 🔍 Troubleshooting

### Display Issues

- **No display**: Check power supply (5V, adequate current)
- **Wrong colors**: Verify RGB pin connections
- **Flickering**: Ensure stable power supply
- **Wrong positioning**: Check row select pin order

### Timer Issues

- **Not counting**: Verify system clock configuration
- **Wrong time**: Check initial time values in countdown.cpp

## 📝 API Reference

### RGBmatrixPanel_STM32 Class

```cpp
void begin();                    // Initialize display
void updateDisplay();           // Refresh display
void fillScreen(uint16_t color); // Clear screen
void setCursor(int16_t x, int16_t y); // Set text position
void setTextColor(uint16_t color);    // Set text color
void print(const char *str);    // Print text
uint16_t Color333(uint8_t r, uint8_t g, uint8_t b); // 3-bit color
uint16_t Color1bit(uint8_t r, uint8_t g, uint8_t b); // 1-bit color
```

## 📄 License

This project is open source. See individual component licenses for details.

## 🤝 Contributing

This project was developed through systematic debugging and optimization. For development history and debugging details, see `DEVELOPMENT_LOG.md`.

---

**Status**: ✅ Complete and fully functional</content>
<parameter name="filePath">c:\Users\rolni\kody\STM32-32x16_RGB_LED\README_new.md
