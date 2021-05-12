#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "logo.h"          // Import custom picture
#include "cross.h"         // Import cross picture
#include "circle.h"        // Import circle picture

#define TFT_GREY 0x5AEB
#define lightblue 0x2D18
#define orange 0xFB60
#define purple 0xFB9B
#define post_yellow 0xFE60

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

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

    
    tft.drawLine(0, 17, 0, 240, post_yellow);
    tft.drawLine(0, 17, 135, 17, TFT_GREY);
    tft.drawLine(134, 17, 134, 240, TFT_GREY);

    // Draw yellow screen
    tft.fillScreen(post_yellow);
  
    //Draw yellow frame
    tft.drawRect(0,0,135,240,BLACK);

    // tft.pushImage(10, 50, 16, 16, cross);
    // tft.pushImage(30, 30, 16, 16, circle);
    tft.drawCircle(30, 30, 16, BLACK);
}

void loop(void)
{

}