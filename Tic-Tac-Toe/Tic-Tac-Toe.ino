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

// Positions from left to right top to bottom
int positionsXY[9][2] = { 
    {15, 50},
    {15, 110},
    {15, 170},
    {55, 50},
    {55, 110},
    {55, 170},
    {95, 50},
    {95, 110},
    {95, 170}
};

int position = 15;
int positionsSet[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

// length of the quadrat in which the x is placed
int lengthX = 25;

int player = 1;

void setup(void)
{
    Serial.begin(9600);
    pinMode(0, INPUT);
    pinMode(35, INPUT);
    tft.init();
    tft.setRotation(0);

    tft.setSwapBytes(true);
    // tft.pushImage(0, 0, 135, 240, bootlogo);

    // tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // tft.setTextSize(0.1);

    // tft.setCursor(0, 0, 2);
    // tft.println("Warum liegt hier stroh");

    
    // tft.drawLine(0, 17, 0, 240, post_yellow);
    // tft.drawLine(0, 17, 135, 17, TFT_GREY);
    // tft.drawLine(134, 17, 134, 240, TFT_GREY);

    // Draw yellow screen
    tft.fillScreen(post_yellow);
  
    // //Draw yellow frame
    // tft.drawRect(0,0,135,240,BLACK);

    // // tft.pushImage(10, 50, 16, 16, cross);
    // // tft.pushImage(30, 30, 16, 16, circle);
    // tft.drawCircle(30, 30, 16, BLACK);

    // horizontal - 
    tft.drawLine(10, 85, 125, 85, BLACK); // first line
    tft.drawLine(10, 155, 125, 155, BLACK); // second line

    // Vertikal |
    tft.drawLine(45, 30, 45, 210, BLACK); // first line
    tft.drawLine(90, 30, 90, 210, BLACK); // second line



    // // loop through the positions array - sizeof needs to be done two times because its a multidimensional array.
    // for ( int i = 0; i < sizeof(positionsXY) / sizeof (positionsXY[0]); ++i ) {
    //     drawX(positionsXY[i][0], positionsXY[i][1], lengthX, BLACK);
    // }
}

void loop()
{
    if (digitalRead(0) == 0) { // button left
        Serial.println("Left Button");
        position = 15;
        if (player == 1) {
            player = 2;
        } else {
            player = 1;
        }
        delay(1000);
    }
    if (digitalRead(35) == 0) { // button right
        Serial.println(position);
        Serial.println(player);
        // for ( int i = 0; i < sizeof(positionsSet); ++i ) {
        //     if (positionsSet[i] == 1) {
        //         position = i;
        //     }
        // }
        if (position == 15) {
            for ( int i = 0; i < sizeof(positionsXY) / sizeof (positionsXY[0]); ++i ) {
                Serial.println(i);
                if (positionsSet[i] == 0) {
                    Serial.println("hi");
                    Serial.println(i);
                    position = i;
                    break;
                }
            }
        } else {
            Serial.println("hi");
            position++;
            // tft.drawRect(, , lengthX, lengthX, post_yellow);
        }
        if (player == 1) {
            Serial.println("player 1");
            drawX(positionsXY[position][0], positionsXY[position][1], lengthX, BLACK);
        } else {
            Serial.println("player 2");
            tft.drawCircle(positionsXY[position][0], positionsXY[position][1], 16, BLACK);
        }
        delay(1000);
    }
}

int drawX(int x, int y, int l, uint32_t color){
    tft.drawLine(x, y, x+l, y+l, color);
    tft.drawLine(x, y+l, x+l, y, color);
    return 0;
}