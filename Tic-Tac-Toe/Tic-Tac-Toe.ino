#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "logo.h"

#define TFT_GREY 0x5AEB
#define lightblue 0x2D18
#define orange 0xFB60
#define purple 0xFB9B

void setup(void)
{
    pinMode(0, INPUT);
    pinMode(35, INPUT);
    tft.init();
    tft.setRotation(0);

    tft.setSwapBytes(true);
    tft.pushImage(0, 0, 135, 240, bootlogo);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(0.1);

    tft.setCursor(0, 0, 2);
    tft.println("Warum liegt hier stroh");
}

void loop(void)
{

}