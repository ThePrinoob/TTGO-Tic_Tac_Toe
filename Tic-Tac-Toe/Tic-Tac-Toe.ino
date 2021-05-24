#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "logo.h"          // Import custom picture
#include "cross.h"         // Import cross picture
#include "circle.h"        // Import circle picture

// Colors
#define post_yellow 0xFE60
#define BLACK   0x0000

// Scene
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

// current position
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

// current player
int player = 1;

void setup(void)
{
    // initialize Serial Output
    Serial.begin(115200);
    
    // Set pin mode of the two inbuilt buttons
    pinMode(0, INPUT);
    pinMode(35, INPUT);

    // Init TFT
    tft.init();
    tft.setRotation(0);

    tft.setSwapBytes(true);

    // tft.pushImage(0, 0, 135, 240, bootlogo);

    // tft.setTextColor(TFT_WHITE, TFT_BLACK);
    // tft.setTextSize(0.1);

    // tft.setCursor(0, 0, 2);
    // tft.println("Warum liegt hier stroh");

    // Draw yellow screen
    tft.fillScreen(post_yellow);

    // draw horizontal lines - 
    tft.drawLine(10, 85, 125, 85, BLACK); // first line
    tft.drawLine(10, 155, 125, 155, BLACK); // second line

    // draw vertical lines |
    tft.drawLine(45, 30, 45, 210, BLACK); // first line
    tft.drawLine(90, 30, 90, 210, BLACK); // second line

}

void loop()
{
    if (digitalRead(0) == 0) { // button left
        Serial.println("Left Button");

        // set the move of the player
        positionsSet[position] = player;

        // When all moves are made go to next scene
        // Will be replaced, because it needs to be checked if a player already won.
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

        position++;

        determinePosition();

        // draw empty space
        for ( int i = 0; i < sizeof(positionsXY) / sizeof (positionsXY[0]); ++i ) {
            if (positionsSet[i] == 0) {
                tft.fillRect(positionsXY[i][0] - 8, positionsXY[i][1] - 5, lengthX + 12, lengthX + 10, post_yellow);
            }
        }

        drawPlayer();

        delay(250);
    }
}

// This function determines the next possible position a player can make
// return void
void determinePosition() {
    // change the position only if its not out of range
    if (position < quantity) {
        // check if the place is already taken.
        for (int i = 0; i < quantity; i++) {
            // If the next field is also taken it starts the function again.
            if (positionsSet[position] != 0) {
                position++;
                determinePosition();
            }
            else {
                break;
            }
        }
    } else {
        // if its out of range it sets the position to zero and searches the first possible move.
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

// Draws the current player at the current position (int) based on the player (int) variable.
// returns void
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
