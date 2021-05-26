#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "logo.h"          // Import custom picture
#include "cross.h"         // Import cross picture
#include "circle.h"        // Import circle picture

// Colors
#define post_yellow 0xFE60
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREY 0x8C51

// Scene
int fase = 1;

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
    {100, 170}};

// current position
int position = 0;

//  x | x | x
//  ---------
//  o | o | o
//  ---------
//  x | x | x
int positionsSet[] = {15, 15, 15,
                      15, 15, 15,
                      15, 15, 15};

int quantity = 8;

// length of the quadrat in which the x is placed
int lengthX = 25;

// current player
int player = 1;

// winner
unsigned int winner;

// score
unsigned int score;

// blinking state of player - 0 hidden, 1 shown
int blink = 0;

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500; //the value is a number of milliseconds used for the blinking player

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

    // Draw yellow screen
    tft.fillScreen(post_yellow);

    // draw horizontal lines -
    tft.drawLine(10, 85, 125, 85, BLACK);   // first line
    tft.drawLine(10, 155, 125, 155, BLACK); // second line

    // draw vertical lines |
    tft.drawLine(45, 30, 45, 210, BLACK); // first line
    tft.drawLine(90, 30, 90, 210, BLACK); // second line

    // Set color, text size and print the current player
    // Text color needs BG, else it doesn't display the number correctly.
    tft.setTextColor(BLACK, post_yellow);
    tft.setTextSize(1);
    tft.setCursor(5, 2, 2);
    tft.println("Spieler: " + String(player));

    startMillis = millis(); //initial start time
}

void loop()
{

    //start screen
    if (fase == 0)
    {
        delay(1000);
        tft.fillScreen(0x0000);
    }

    //game screen
    else if (fase == 1)
    {
        if (digitalRead(0) == 0)
        { // button left
            Serial.println("Left Button");

            drawPlayer();

            // set the move of the player
            positionsSet[position] = player;

            score = positionsSet[0] + positionsSet[1] + positionsSet[2];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[3] + positionsSet[4] + positionsSet[5];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[6] + positionsSet[7] + positionsSet[8];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[0] + positionsSet[3] + positionsSet[6];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[1] + positionsSet[4] + positionsSet[7];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[2] + positionsSet[5] + positionsSet[8];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[0] + positionsSet[4] + positionsSet[8];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            score = positionsSet[2] + positionsSet[4] + positionsSet[6];
            if (score == 3 || score == 6)
            {
                winner = (score == 3) ? 1 : 2;
            }

            if (!winner)
            {
                // When all moves are made go to next scene and display draw
                // 
                for (int i = 0; i <= quantity; ++i)
                {
                    if (positionsSet[i] == 15)
                    {
                        break;
                    }
                    else
                    {
                        if (i == quantity)
                        {
                            Serial.println(i);
                            Serial.println(quantity);
                            fase++;
                            winner = 3;
                            nextScene(fase);
                        }
                    }
                }

                // reset the position
                position = 0;

                // change the player
                if (player == 1)
                {
                    player = 2;
                }
                else
                {
                    player = 1;
                }

                tft.setCursor(5, 2, 2);
                tft.println("Spieler: " + String(player));

                determinePosition();

                drawPlayer();

                drawEmptySpace();

                delay(250);
            }
            else
            {
                fase++;
                nextScene(fase);
            }
        }

        if (digitalRead(35) == 0)
        { // button right
            Serial.println(position);
            Serial.println(player);

            position++;

            determinePosition();

            drawEmptySpace();

            drawPlayer();

            delay(250);
        }

        currentMillis = millis(); //get the current "time" (actually the number of milliseconds since the program started)

        if (currentMillis - startMillis >= period) //test whether the period has elapsed
        {
            if (blink == 0)
            {
                drawEmptySpace();
                blink = 1;
            }
            else
            {
                determinePosition();
                drawPlayer();
                blink = 0;
            }
            startMillis = currentMillis; // save the start time of the current state.
        }
    }
    else if (fase == 2)
    {
        //to do
        setEndScreen();
    }
}

void drawEmptySpace()
{
    // draw empty space
    for (int i = 0; i < sizeof(positionsXY) / sizeof(positionsXY[0]); ++i)
    {
        if (positionsSet[i] == 15)
        {
            tft.fillRect(positionsXY[i][0] - 8, positionsXY[i][1] - 5, lengthX + 12, lengthX + 10, post_yellow);
        }
    }
}

// This function determines the next possible position a player can make
// return void
void determinePosition()
{
    // change the position only if its not out of range
    if (position <= quantity)
    {
        // check if the place is already taken.
        for (int i = 0; i <= quantity; i++)
        {
            // If the next field is also taken it starts the function again.
            if (positionsSet[position] != 15)
            {
                position++;
                determinePosition();
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        // if its out of range it sets the position to zero and searches the first possible move.
        position = 0;
        for (int i = 0; i <= quantity; i++)
        {
            if (positionsSet[position] != 15)
            {
                position++;
            }
            else
            {
                break;
            }
        }
    }
}

// Draws the current player at the current position (int) based on the player (int) variable.
// returns void
void drawPlayer()
{
    if (player == 1)
    {
        Serial.println("player 1");
        drawX(positionsXY[position][0], positionsXY[position][1], lengthX, BLACK);
    }
    else
    {
        Serial.println("player 2");
        tft.drawCircle(positionsXY[position][0] + 12, positionsXY[position][1] + 10, 14, BLACK);
    }
}

// Draws a X with the given coordinates and color
void drawX(int x, int y, int l, uint32_t color)
{
    tft.drawLine(x, y, x + l, y + l, color);
    tft.drawLine(x, y + l, x + l, y, color);
}

// TBD
void nextScene(int sceneNumber)
{
    tft.fillScreen(BLACK);
}

void setEndScreen()
{
    if (winner == 1)
    {
        tft.fillScreen(BLUE);
    }
    else if (winner == 2)
    {
        tft.fillScreen(RED);
    }
    else if (winner == 3)
    {
        tft.fillScreen(GREY);
    }
}
