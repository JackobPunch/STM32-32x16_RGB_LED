#include "main.h"
#include "RGBmatrixPanel_STM32.h"
#include "countdown.h"
#include <cstdio>

RGBmatrixPanel_STM32 matrix(false); // Global instance

unsigned long previousMillis = 0;
int seconds = 0;
int minutes = 3;
bool finished = false;
bool paused = true;               // Start paused at 03:00
bool display_needs_update = true; // Track if display needs refreshing
bool first_run = true;            // Flag for first countdown() call

void countdown_init()
{
    matrix.begin();
    HAL_Delay(50);                  // Give display hardware time to stabilize
    previousMillis = HAL_GetTick(); // Initialize timing
}

void countdown()
{
    unsigned long currentMillis = HAL_GetTick();

    if (currentMillis - previousMillis >= 1000 && !paused && !finished)
    {
        previousMillis = currentMillis;

        if (!finished)
        {
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

        // Update display when time changes
        update_display();
    }

    // Force initial display update on first run to clear random colors
    if (first_run)
    {
        update_display();
        first_run = false;
    }

    // Handle display updates for button presses (when not counting)
    if (display_needs_update && paused)
    {
        update_display();
        display_needs_update = false;
    }

    // Continuous display refresh for LED matrix
    matrix.updateDisplay();
}

void countdown_start()
{
    paused = false;
    previousMillis = HAL_GetTick() - 1000; // Ensure no immediate decrement
}

void countdown_stop()
{
    paused = true;
    display_needs_update = true;
}

void countdown_reset()
{
    paused = true;
    finished = false;
    minutes = 3;
    seconds = 0;
    previousMillis = HAL_GetTick();
    display_needs_update = true;
}

void update_display()
{
    // Always clear the entire display first
    matrix.fillScreen(0);

    // Display countdown timer at the bottom in red
    matrix.setCursor(3, 9);
    matrix.setTextColor(RED); // Red
    char minStr[3];
    sprintf(minStr, "%02d", minutes);
    matrix.print(minStr);

    matrix.setCursor(13, 9);
    matrix.print(":");

    matrix.setCursor(17, 9);
    char secStr[3];
    sprintf(secStr, "%02d", seconds);
    matrix.print(secStr);

    // Always display "TIMER" at the top in green
    matrix.setCursor(1, 1);
    matrix.setTextColor(GREEN); // Green
    matrix.print("TIMER");

    // Note: matrix.updateDisplay() is now called continuously in main loop
}