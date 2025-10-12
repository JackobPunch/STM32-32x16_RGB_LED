#include "main.h"
#include "hub75.h"
#include "frame.h"
#include "pixel.h"

// Simple 5x7 font bitmaps for digits 0-9 (each digit is 5 columns x 7 rows)
const uint8_t digitBitmaps[10][35] = {
    // 0
    {0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     0, 1, 1, 1, 0},
    // 1
    {0, 0, 1, 0, 0,
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 0,
     0, 1, 1, 1, 0},
    // 2
    {0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     0, 0, 0, 0, 1,
     0, 0, 0, 1, 0,
     0, 0, 1, 0, 0,
     0, 1, 0, 0, 0,
     1, 1, 1, 1, 1},
    // 3
    {0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     0, 0, 0, 0, 1,
     0, 0, 1, 1, 0,
     0, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     0, 1, 1, 1, 0},
    // 4
    {0, 0, 0, 0, 1,
     0, 0, 0, 1, 1,
     0, 0, 1, 0, 1,
     0, 1, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1},
    // 5
    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 0,
     0, 0, 0, 0, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 0},
    // 6
    {0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     0, 1, 1, 1, 0},
    // 7
    {1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     0, 0, 0, 1, 0,
     0, 0, 1, 0, 0,
     0, 1, 0, 0, 0,
     0, 1, 0, 0, 0,
     0, 1, 0, 0, 0},
    // 8
    {0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     0, 1, 1, 1, 0},
    // 9
    {0, 1, 1, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     0, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     0, 0, 0, 0, 1,
     0, 1, 1, 1, 0}};

// Function to draw a digit at (x,y) on the frame
void drawDigit(HUB75::Frame<32, 16, HUB75::Pixel8bit> &frame, int x, int y, int digit, HUB75::Pixel8bit color)
{
    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (digitBitmaps[digit][row * 5 + col])
            {
                frame.pixel(x + col, y + row) = color;
            }
        }
    }
}

// Simple 5x7 font bitmaps for letters E N D
const uint8_t letterBitmaps[3][35] = {
    // E
    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 0, 0,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1},
    // N
    {1, 0, 0, 0, 1,
     1, 1, 0, 0, 1,
     1, 0, 1, 0, 1,
     1, 0, 0, 1, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1},
    // D
    {1, 1, 1, 0, 0,
     1, 0, 0, 1, 0,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 1, 0,
     1, 1, 1, 0, 0}};

// Function to draw a letter at (x,y) on the frame
void drawLetter(HUB75::Frame<32, 16, HUB75::Pixel8bit> &frame, int x, int y, int letter, HUB75::Pixel8bit color)
{
    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 5; col++)
        {
            if (letterBitmaps[letter][row * 5 + col])
            {
                frame.pixel(x + col, y + row) = color;
            }
        }
    }
}

// Function to draw colon at (x,y) on the frame
void drawColon(HUB75::Frame<32, 16, HUB75::Pixel8bit> &frame, int x, int y, HUB75::Pixel8bit color)
{
    frame.pixel(x, y) = color;
    frame.pixel(x, y + 2) = color;
}

// Countdown function
extern "C" void countdown()
{
    // Commented out countdown logic for stable display test
    /*
    static unsigned long previousMillis = 0;
    static int minutes = 3;
    static int seconds = 0;
    static bool finished = false;
    static int refreshCounter = 0;
    */

    // Initialize HUB75 matrix (static to init once)
    static HUB75::HUB75<
        32, 16,
        HUB75::PinRow<GPIOA_BASE, 5, 6>, // Data PA5-PA10
        HUB75::PinRow<GPIOB_BASE, 3, 4>, // Row PB3-PB6
        HUB75::Pin<GPIOB_BASE, 10>,      // CLK PB10
        HUB75::Pin<GPIOB_BASE, 11>,      // LAT PB11
        HUB75::Pin<GPIOB_BASE, 12>       // OE PB12
        >
        matrix;

    static HUB75::Frame<32, 16, HUB75::Pixel8bit> frame;
    static HUB75::Pixel8bit red(255, 0, 0);
    static HUB75::Pixel8bit green(0, 255, 0);

    // Initial display on first call
    static bool initialized = false;
    if (!initialized)
    {
        frame.fill(HUB75::Pixel8bit(0, 0, 0)); // Clear
        drawDigit(frame, 1, 4, 0, red);
        drawDigit(frame, 7, 4, 3, red);
        drawColon(frame, 13, 5, red);
        drawDigit(frame, 14, 4, 0, red);
        drawDigit(frame, 20, 4, 0, red);
        matrix.display(frame);
        HAL_Delay(3000);
        initialized = true;
    }

    // Static display for testing
    frame.fill(HUB75::Pixel8bit(0, 0, 0));
    drawDigit(frame, 1, 4, 0, red);
    drawDigit(frame, 7, 4, 3, red);
    drawColon(frame, 13, 5, red);
    drawDigit(frame, 14, 4, 0, red);
    drawDigit(frame, 20, 4, 0, red);

    // Display continuously, but throttled to reduce flickering
    static int refreshCounter = 0;
    refreshCounter++;
    if (refreshCounter >= 1)
    {
        matrix.display(frame);
        refreshCounter = 0;
        // No delay for faster refresh
    }
    /*
    unsigned long currentMillis = HAL_GetTick();
    if (currentMillis - previousMillis >= 1000)
    {
        previousMillis = currentMillis;

        if (seconds == 0)
        {
            if (minutes == 0)
            {
                finished = true;
            }
            else
            {
                minutes--;
                seconds = 59;
            }
        }
        else
        {
            seconds--;
        }
    }

    frame.fill(HUB75::Pixel8bit(0, 0, 0));
    if (!finished)
    {
        // Draw MM:SS
        drawDigit(frame, 1, 4, minutes / 10, red); // Minutes tens
        drawDigit(frame, 7, 4, minutes % 10, red); // Minutes units
        drawColon(frame, 13, 5, red);
        drawDigit(frame, 14, 4, seconds / 10, red); // Seconds tens
        drawDigit(frame, 20, 4, seconds % 10, red); // Seconds units
    }
    else
    {
        // Draw "END"
        drawLetter(frame, 6, 4, 0, green);  // E
        drawLetter(frame, 12, 4, 1, green); // N
        drawLetter(frame, 18, 4, 2, green); // D
    }

    // Display continuously, but throttled to reduce flickering
    refreshCounter++;
    if (refreshCounter >= 100)
    {
        matrix.display(frame);
        refreshCounter = 0;
    }
    */
}