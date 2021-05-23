#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "logo.h"          // Import custom picture
#include "cross.h"         // Import cross picture
#include "circle.h"        // Import circle picture

#define post_yellow 0xFE60
#define BLACK   0x0000

int scene = 0;

// Positions from left to right top to bottom
int positionsXY[9][2] = { 
    {15, 50},
    {55, 50},
    {100, 50},
    {15, 110},
    {55, 110},
    {100, 110},
    {15, 170},
    {55, 170},
    {100, 170}
};

int position = -1;

                    //  x | x | x
                    //  ---------
                    //  o | o | o
                    //  ---------
                    //  x | x | x
int positionsSet[] = {  0, 0, 0,
                        0, 0, 0,
                        0, 0, 0
                    };

int quantity = sizeof(positionsXY) / sizeof (positionsXY[0]);

// length of the quadrat in which the x is placed
int lengthX = 25;

int player = 1;

void setup(void)
{
    Serial.begin(115200);
    
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

        // set the move of the player
        positionsSet[position] = player;

        // When all moves are made go to next scene
        for ( int i = 0; i < quantity; ++i ) {
            if (positionsSet[i] == 0) {
                break;
            } else {
                if (i == quantity) {
                    scene++;
                    nextScene(scene);
                }
            }
        }
        // reset the position
        position = -1;

        // change the player
        if (player == 1) {
            player = 2;
        } else {
            player = 1;
        }

        determinePosition();

        drawPlayer();

        delay(250);
    }

    if (digitalRead(35) == 0) { // button right
        Serial.println(position);
        Serial.println(player);

        buttonRight();

        delay(250);
    }
}

void buttonRight() {
    
    position++;
    determinePosition();

    // draw empty space - could be solved better
    for ( int i = 0; i < sizeof(positionsXY) / sizeof (positionsXY[0]); ++i ) {
        if (positionsSet[i] == 0) {
            tft.fillRect(positionsXY[i][0] - 8, positionsXY[i][1] - 5, lengthX + 12, lengthX + 10, post_yellow);
        }
    }

    drawPlayer();
}

void determinePosition() {
    // change the position only if its not out of range
    if (position < quantity) {
        // check if the place is already taken.
        for (int i = 0; i < quantity; i++) {
            if (positionsSet[position] != 0) {
                position++;
                determinePosition();
            }
            else {
                break;
            }
        }
    } else {
        position = 0;
        for (int i = 0; i < quantity; i++) {
            if (positionsSet[position] != 0) {
                position++;
            }
            else {
                break;
            }
        }
    }
}

void drawPlayer() {
    if (player == 1) {
        Serial.println("player 1");
        drawX(positionsXY[position][0], positionsXY[position][1], lengthX, BLACK);
    } else {
        Serial.println("player 2");
        tft.drawCircle(positionsXY[position][0] + 12, positionsXY[position][1] + 10, 14, BLACK);
    }
}

// Draws a X with the given coordinates and color
void drawX(int x, int y, int l, uint32_t color){
    tft.drawLine(x, y, x+l, y+l, color);
    tft.drawLine(x, y+l, x+l, y, color);
}

// TBD
void nextScene(int sceneNumber) {
    tft.fillScreen(BLACK);
}
