#ifndef RGBMATRIXPANEL_STM32_H
#define RGBMATRIXPANEL_STM32_H

#include "stm32f4xx_hal.h"
// #include "Adafruit_GFX.h" // Removed for simplicity

// For 32x16 matrix: 32 columns, 16 rows, 1/8 scan
#define WIDTH 32
#define HEIGHT 16
#define NROWS 16
#define NCOLS 32

class RGBmatrixPanel_STM32
{
public:
    RGBmatrixPanel_STM32(bool dbuf = false);
    void begin(void);
    void updateDisplay(void);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void fillScreen(uint16_t color);
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t color);
    void print(const char *str);
    uint16_t Color333(uint8_t r, uint8_t g, uint8_t b);
    void drawDigit(int x, int y, int digit, uint16_t color);

private:
    bool doublebuffer;
    uint8_t *matrixbuff[2];
    uint8_t backindex;
    volatile uint8_t *buffptr;
    volatile bool swapflag;
    volatile uint8_t row, plane;
    uint8_t nRows, nPlanes;
    int16_t cursor_x, cursor_y;
    uint16_t text_color;
};

#endif