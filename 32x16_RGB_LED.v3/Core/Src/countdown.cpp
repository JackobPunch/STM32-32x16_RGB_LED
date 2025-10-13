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
bool ready_alternate = false;     // Toggle between "READY" and "ST8DY"

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

    // Handle ready state alternating display
    if (paused && minutes == 3 && seconds == 0 && !finished)
    {
        static unsigned long last_toggle = 0;
        if (currentMillis - last_toggle >= 1000)
        {
            last_toggle = currentMillis;
            ready_alternate = !ready_alternate;
            update_display();
        }
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
    ready_alternate = false; // Reset to show "READY" first
    previousMillis = HAL_GetTick();
    display_needs_update = true;
}

void update_display()
{
    // Always clear the entire display first
    matrix.fillScreen(0);

    // Check timer states
    bool is_ready_state = (paused && minutes == 3 && seconds == 0 && !finished);
    bool is_stopped_state = (paused && !(minutes == 3 && seconds == 0) && !finished);
    bool is_finished_state = finished;

    if (is_ready_state)
    {
        // Ready state: Yellow "READY?" + Blue countdown
        matrix.setCursor(3, 9);
        matrix.setTextColor(BLUE); // Blue
        char minStr[3];
        sprintf(minStr, "%02d", minutes);
        matrix.print(minStr);

        matrix.setCursor(13, 9);
        matrix.print(":");

        matrix.setCursor(17, 9);
        char secStr[3];
        sprintf(secStr, "%02d", seconds);
        matrix.print(secStr);

        // Display alternating "READY"/"ST8DY" at the top in yellow
        matrix.setCursor(1, 1);
        matrix.setTextColor(YELLOW); // Yellow
        if (ready_alternate)
        {
            matrix.print("ST8DY");
        }
        else
        {
            matrix.print("READY");
        }
    }
    else if (is_stopped_state)
    {
        // Stopped state: Yellow countdown + Red "STOP"
        matrix.setCursor(3, 9);
        matrix.setTextColor(YELLOW); // Yellow
        char minStr[3];
        sprintf(minStr, "%02d", minutes);
        matrix.print(minStr);

        matrix.setCursor(13, 9);
        matrix.print(":");

        matrix.setCursor(17, 9);
        char secStr[3];
        sprintf(secStr, "%02d", seconds);
        matrix.print(secStr);

        // Display "STOP" at the top in red
        matrix.setCursor(4, 1);
        matrix.setTextColor(RED); // Red
        matrix.print("STOP");
    }
    else if (is_finished_state)
    {
        // Finished state: Yellow "00:00" + Red "STOP"
        matrix.setCursor(3, 9);
        matrix.setTextColor(YELLOW); // Yellow
        char minStr[3];
        sprintf(minStr, "%02d", minutes);
        matrix.print(minStr);

        matrix.setCursor(13, 9);
        matrix.print(":");

        matrix.setCursor(17, 9);
        char secStr[3];
        sprintf(secStr, "%02d", seconds);
        matrix.print(secStr);

        // Display "STOP" at the top in red
        matrix.setCursor(4, 1);
        matrix.setTextColor(RED); // Red
        matrix.print("STOP");
    }
    else
    {
        // Active/Counting state: Red countdown + Green "TIMER"
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

        // Display "GO!!!" at the top in green
        matrix.setCursor(2, 1);
        matrix.setTextColor(GREEN); // Green
        matrix.print("GO!!!");
    }

    // Note: matrix.updateDisplay() is now called continuously in main loop
}