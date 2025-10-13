#include "main.h"
#include "RGBmatrixPanel_STM32.h"
#include "countdown.h"
#include <cstdio>

RGBmatrixPanel_STM32 matrix(false); // Global instance

void countdown_init()
{
    matrix.begin();
}

unsigned long previousMillis = 0;
int seconds = 0;
int minutes = 3;
bool finished = false;

void countdown()
{
    unsigned long currentMillis = HAL_GetTick();

    if (currentMillis - previousMillis >= 1000)
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

        matrix.fillScreen(0);

        // Display countdown timer at the bottom in red
        matrix.setCursor(3, 9);
        matrix.setTextColor(matrix.Color333(7, 0, 0)); // Red
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
        matrix.setTextColor(matrix.Color333(0, 7, 0)); // Green
        matrix.print("TIMER");
    }

    matrix.updateDisplay();
}