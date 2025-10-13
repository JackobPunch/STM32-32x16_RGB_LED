#include "RGBmatrixPanel_STM32.h"
#include "stm32f4xx_hal.h"
#include <cstdlib>

#define LOOPTIME 8000 // Adjust for STM32 clock
#define CALLOVERHEAD 200

RGBmatrixPanel_STM32::RGBmatrixPanel_STM32(bool dbuf)
    : doublebuffer(dbuf)
{
    nRows = NROWS;
    nPlanes = 1; // Simplified, no BCM for now
    backindex = 0;
    buffptr = NULL;
    swapflag = false;
    row = 0;
    plane = 0;
    cursor_x = 0;
    cursor_y = 0;
    text_color = 0xFFFF; // White

    // Allocate buffers: HEIGHT rows * (WIDTH * 3 / 8) bytes per row
    int buffsize = HEIGHT * (WIDTH * 3 / 8);
    matrixbuff[0] = (uint8_t *)malloc(buffsize);
    if (doublebuffer)
    {
        matrixbuff[1] = (uint8_t *)malloc(buffsize);
    }
    else
    {
        matrixbuff[1] = matrixbuff[0];
    }
    buffptr = matrixbuff[backindex];
}

void RGBmatrixPanel_STM32::begin(void)
{
    // Buffers already allocated, pins initialized in MX_GPIO_Init
}

void RGBmatrixPanel_STM32::updateDisplay(void)
{
    // Disable OE
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // OE high

    // Latch prior data
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); // LAT high

    // Simplified for static display, no planes
    uint8_t next_row = row + 1;
    if (next_row >= nRows)
    {
        next_row = 0;
        if (swapflag)
        {
            backindex = 1 - backindex;
            swapflag = false;
        }
        buffptr = matrixbuff[1 - backindex];
    }
    row = next_row;

    // Set row address A B C, D=0 for 8 rows
    uint8_t row_addr = 7 - row;                                                             // Reverse row bits
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (row_addr & 0x1) ? GPIO_PIN_SET : GPIO_PIN_RESET); // A
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (row_addr & 0x2) ? GPIO_PIN_SET : GPIO_PIN_RESET); // B
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (row_addr & 0x4) ? GPIO_PIN_SET : GPIO_PIN_RESET); // C
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);                                   // D

    // Enable OE
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); // OE low

    // Latch down
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); // LAT low

    // Load data using direct register for speed
    int bytes_per_row = WIDTH * 3 / 8;
    uint8_t upper_row = 7 - row;                                       // Reverse upper half
    uint8_t lower_row = 15 - row;                                      // Reverse lower half
    uint8_t *ptr = matrixbuff[backindex] + upper_row * bytes_per_row;  // Upper row
    uint8_t *ptr2 = matrixbuff[backindex] + lower_row * bytes_per_row; // Lower row
    GPIO_TypeDef *dataPort = GPIOA;                                    // PA5-PA10
    uint32_t clkPin = GPIO_PIN_10;                                     // PB10
    GPIO_TypeDef *clkPort = GPIOB;

    for (int col = 0; col < WIDTH; col++)
    {
        uint8_t r1 = (ptr[col / 8 * 3 + 0] >> (7 - (col % 8))) & 1; // MSB first
        uint8_t g1 = (ptr[col / 8 * 3 + 1] >> (7 - (col % 8))) & 1;
        uint8_t b1 = (ptr[col / 8 * 3 + 2] >> (7 - (col % 8))) & 1;
        uint8_t r2 = (ptr2[col / 8 * 3 + 0] >> (7 - (col % 8))) & 1;
        uint8_t g2 = (ptr2[col / 8 * 3 + 1] >> (7 - (col % 8))) & 1;
        uint8_t b2 = (ptr2[col / 8 * 3 + 2] >> (7 - (col % 8))) & 1;

        // Set data pins PA5=R1, PA6=G1, PA7=B1, PA8=R2, PA9=G2, PA10=B2
        dataPort->ODR = (dataPort->ODR & ~(0x3F << 5)) | ((r1 << 5) | (g1 << 6) | (b1 << 7) | (r2 << 8) | (g2 << 9) | (b2 << 10));

        // CLK pulse
        clkPort->BSRR = clkPin;
        clkPort->BSRR = (uint32_t)clkPin << 16;
    }
}

// drawPixel implementation
void RGBmatrixPanel_STM32::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;

    // Extract RGB from 565 color
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    // For simplicity, threshold to 1 bit per color
    uint8_t r_bit = (r > 15) ? 1 : 0;
    uint8_t g_bit = (g > 31) ? 1 : 0;
    uint8_t b_bit = (b > 15) ? 1 : 0;

    // Calculate buffer position: HEIGHT rows * (WIDTH * 3 / 8) bytes per row
    int bytes_per_row = WIDTH * 3 / 8;
    uint8_t *ptr = matrixbuff[backindex] + y * bytes_per_row + (x / 8 * 3);
    uint8_t bit = 7 - (x % 8); // MSB first
    uint8_t mask = 1 << bit;

    if (r_bit)
        ptr[0] |= mask;
    else
        ptr[0] &= ~mask;

    if (g_bit)
        ptr[1] |= mask;
    else
        ptr[1] &= ~mask;

    if (b_bit)
        ptr[2] |= mask;
    else
        ptr[2] &= ~mask;
}

// GFX-like methods
void RGBmatrixPanel_STM32::fillScreen(uint16_t color)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            drawPixel(x, y, color);
        }
    }
}

void RGBmatrixPanel_STM32::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void RGBmatrixPanel_STM32::setTextColor(uint16_t color)
{
    text_color = color;
}

void RGBmatrixPanel_STM32::print(const char *str)
{
    int x = cursor_x;
    int y = cursor_y;
    while (*str)
    {
        if (*str >= '0' && *str <= '9')
        {
            drawDigit(x, y, *str - '0', text_color);
            x += 6;
        }
        else if (*str >= 'A' && *str <= 'Z')
        {
            drawChar(x, y, *str, text_color);
            x += 6;
        }
        else if (*str == ':')
        {
            drawPixel(x + 2, y + 2, text_color);
            drawPixel(x + 2, y + 4, text_color);
            x += 4;
        }
        else if (*str == ' ')
        {
            x += 6;
        }
        str++;
    }
}

uint16_t RGBmatrixPanel_STM32::Color333(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0x7) << 13) | ((g & 0x7) << 8) | ((b & 0x3) << 3);
}

uint16_t RGBmatrixPanel_STM32::Color1bit(uint8_t r, uint8_t g, uint8_t b)
{
    // r,g,b are 0 or 1
    return ((r & 0x1) << 15) | ((g & 0x1) << 10) | ((b & 0x1) << 5);
}

void RGBmatrixPanel_STM32::drawDigit(int x, int y, int digit, uint16_t color)
{
    // Copy from old code
    const uint8_t digitBitmaps[10][35] = {
        {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // 0
        {0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0}, // 1
        {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1}, // 2
        {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // 3
        {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, // 4
        {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // 5
        {0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // 6
        {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}, // 7
        {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // 8
        {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}  // 9
    };
    for (int i = 0; i < 35; i++)
    {
        int dx = i % 5;
        int dy = i / 5;
        if (digitBitmaps[digit][i])
        {
            drawPixel(x + dx, y + dy, color);
        }
    }
}

void RGBmatrixPanel_STM32::drawChar(int x, int y, char c, uint16_t color)
{
    if (c >= 'A' && c <= 'Z')
    {
        int index = c - 'A';
        const uint8_t letterBitmaps[26][35] = {
            {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, // A
            {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0}, // B
            {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // C
            {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0}, // D
            {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // E
            {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}, // F
            {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // G
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, // H
            {0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0}, // I
            {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // J
            {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1}, // K
            {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // L
            {1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, // M
            {1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, // N
            {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // O
            {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0}, // P
            {0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1}, // Q
            {1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1}, // R
            {0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0}, // S
            {1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}, // T
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0}, // U
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0}, // V
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1}, // W
            {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1}, // X
            {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}, // Y
            {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}  // Z
        };
        for (int i = 0; i < 35; i++)
        {
            int dx = i % 5;
            int dy = i / 5;
            if (letterBitmaps[index][i])
            {
                drawPixel(x + dx, y + dy, color);
            }
        }
    }
}
