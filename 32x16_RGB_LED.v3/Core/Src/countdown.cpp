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
    if (finished)
        return;

    unsigned long currentMillis = HAL_GetTick();

    if (currentMillis - previousMillis >= 1000)
    {
        previousMillis = currentMillis;

        if (seconds == 0)
        {
            if (minutes == 0)
            {
                finished = true;
                matrix.fillScreen(0);
                matrix.setCursor(8, 1);
                matrix.setTextColor(matrix.Color333(0, 7, 0)); // Green
                matrix.print("END");
                return;
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

        matrix.fillScreen(0);

        char timeStr[6];
        sprintf(timeStr, "%02d:%02d", minutes, seconds);
        matrix.setCursor(1, 1);
        matrix.setTextColor(matrix.Color333(7, 0, 0)); // Red
        matrix.print(timeStr);
    }

    matrix.updateDisplay();
}