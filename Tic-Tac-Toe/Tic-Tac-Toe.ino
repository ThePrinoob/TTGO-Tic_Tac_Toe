#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "logo.h"

void setup(void)
{
    pinMode(0, INPUT);
    pinMode(35, INPUT);
    tft.init();
    tft.setRotation(0);

    tft.setSwapBytes(true);
    tft.pushImage(0, 0, 135, 240, bootlogo);
}

void loop(void)
{
    
}