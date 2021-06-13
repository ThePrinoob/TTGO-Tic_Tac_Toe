#include <SPI.h>
#include <TFT_eSPI.h>           // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();      // Invoke custom library
#include "esp_wifi.h"           // Wifi library - used for two player function
#include "ESPAsyncWebServer.h"  // Async Webserver Library - used for two player function on host side
#include <HTTPClient.h>         // Client Library - - used for two player function on client side

// Colors
#define post_yellow 0xFE60

// Scene
int fase = 0;

// position of arrow on main screen
int arrowPosition = 1;

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

                //    x | x | x
                //    ---------
                //    o | o | o
                //    ---------
                //    x | x | x
int positionsSet[] = {15, 15, 15,
                      15, 15, 15,
                      15, 15, 15};

// number of fields
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

// millis part of blinking player and get requests of the client
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500; //the value is a number of milliseconds used for the blinking player and get requests of the client

// first run - set to false as soon the function was run once
bool firstRun = true;

// --------------------------------
// Online part
// --------------------------------

// Set your access point network credentials
const char* ssid = "ESP32-TicTacToe";
const char* password = "ticTacToe";

// URL's for get and post requests
const char* serverGameGetPlayer = "http://192.168.4.1/gameGetPlayer";
const char* serverGameGetPositions = "http://192.168.4.1/gameGetPositions";
const char* serverGameSetPlayer = "http://192.168.4.1/gamePostPlayer";
const char* serverGameSetPositions = "http://192.168.4.1/gamePostPositions";
bool isServer = true;
bool isOnline = true;

// init AsyncWebServer on Port 80
AsyncWebServer server(80);

void setup(void)
{
    // initialize Serial Output
    Serial.begin(115200);

    // Set pin mode of the two inbuilt buttons
    pinMode(0, INPUT);
    pinMode(35, INPUT);

    // Init TFT and screen
    tft.init();
    tft.setRotation(0);
    tft.setSwapBytes(true);
    
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(post_yellow, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(5, 40, 2);

    tft.println("TicTacToe");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    
    tft.setTextSize(1);
    
    tft.setCursor(35, 100, 2);
    tft.println("Host Game");

    tft.setCursor(35, 150, 2);
    tft.println("Join Game");
    
    tft.setCursor(35, 200, 2);
    tft.println("Local Game");

    tft.setCursor(10, 102, 1);
    tft.println("->");

    startMillis = millis(); //initial start time
}

void loop()
{
    //start screen
    if (fase == 0)
    {
        if (digitalRead(0) == 0)
        {
            // execute the game pickt at the arrow position.
            tft.fillScreen(TFT_BLACK);

            tft.setCursor(5, 103, 2);

            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            if (arrowPosition == 1)
            {
                isOnline = true;
                isServer = true;
                setupServer();
            }
            else if (arrowPosition == 2)
            {
                isOnline = true;
                isServer = false;
                setupClient();
            }
            else if (arrowPosition == 3)
            {
                isOnline = false;
            }
            fase += 1;
        }

        if (digitalRead(35) == 0)
        { // button right
            if (arrowPosition < 3)
            {
                // delete the arrow at the old position and set it at the new position.
                int position = 51 + (51 * arrowPosition);
                int newPosition = 51 + (51 * (arrowPosition + 1));

                tft.setCursor(10, position, 1);
                tft.println("  ");

                tft.setCursor(10, newPosition, 1);
                tft.println("->");

                arrowPosition++;
            }
            else
            {
                int position = 51 + (51 * arrowPosition);
                arrowPosition = 1;
                int newPosition = 51 + (51 * arrowPosition);

                tft.setCursor(10, position, 1);
                tft.println("  ");

                tft.setCursor(10, newPosition, 1);
                tft.println("->");
            }
            delay(250);
        }
    }

    // game screen
    else if (fase == 1 && !winner)
    {
        /*
            Fase 1 is the main fase. It manages the whole game and Button clicks.
        */
        currentMillis = millis(); //get the current "time" (actually the number of milliseconds since the program started)

        if(firstRun) {
            // Draw yellow screen
            tft.fillScreen(post_yellow);

            // draw horizontal lines -
            tft.drawLine(10, 85, 125, 85, TFT_BLACK);   // first line
            tft.drawLine(10, 155, 125, 155, TFT_BLACK); // second line

            // draw vertical lines |
            tft.drawLine(45, 30, 45, 210, TFT_BLACK); // first line
            tft.drawLine(90, 30, 90, 210, TFT_BLACK); // second line

            // Set color, text size and print the current player
            // Text color needs BG, else it doesn't display the number correctly.
            tft.setTextColor(TFT_BLACK, post_yellow);
            tft.setTextSize(1);
            tft.setCursor(5, 2, 2);
            tft.println("Spieler: " + String(player));
            firstRun = false;
            delay(1000);
        }

        // millis part - checks every 500ms if its a client and if the enemy has played
        // Furthermore it lets the Player blink.
        if (currentMillis - startMillis >= period) //test whether the period has elapsed
        {
            if (isOnline) {
                if (!isServer) {
                    // Get the current Player and test if it has changed. On change it will get the positions and draws the player.
                    player = httpGETRequest(serverGameGetPlayer).toInt();
                    Serial.println(String(player));
                    if (player > 1) {
                        convertToArray(httpGETRequest(serverGameGetPositions));
                        drawAllPlayers();
                    }
                    if(WiFi.status() == WL_CONNECTED) {
                        displayInternetConnectionStatus(true);
                    } else {
                        displayInternetConnectionStatus(false);
                    }
                } else {
                    if (WiFi.softAPgetStationNum() == 1) {
                        displayInternetConnectionStatus(true);
                    } else {
                        displayInternetConnectionStatus(false);
                    }
                }
            }

            if (!isServer && player == 2 || isServer && player == 1 || !isOnline) {
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
            } else {
                blink = 0;
            }

            startMillis = currentMillis; // save the start time of the current state.
        }

        if (isOnline) {
            // Deactivate (goes back to void()) all functions when its not the clients or the Hosts turn.
            if (!isServer && player == 1 || isServer && player == 2) {
                return;
            }
        }
        
        if (digitalRead(0) == 0)
        { // button left
            Serial.println("Left Button");

            drawPlayer();

            // set the move of the player
            positionsSet[position] = player;

            checkForWinner(true);

            // check if somebody won - if nobody won it checks for a draw.
            if (!winner)
            {
                checkForDraw();
                // even when its a draw the winner value is set and if its set it doesn't run the following code.
                if(winner) { return; }
            } else {
                return;
            }

            // reset the position
            position = 0;

            // change the player
            player = (player == 1) ? 2 : 1;

            // Display current player
            tft.setCursor(5, 2, 2);
            tft.println("Spieler: " + String(player));

            determinePosition();

            drawPlayer();

            drawEmptySpace();

            // Send the player and the positions to the host
            if (!isServer && isOnline) {
                httpPOSTRequest(serverGameSetPlayer, String(player));
                String tmpPositionsSet = convertToString();
                httpPOSTRequest(serverGameSetPositions, tmpPositionsSet);
            }

            delay(250);
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
    }
    else if (fase == 2)
    {
        /*
            Fase 2 manages the end screen and displays the winner or if its a draw.
            Reset TTGO to play again!
        */
        if (firstRun) {
            tft.setCursor(13, 103, 2);
            tft.setTextSize(1);
            if (winner == 1)
            {
                tft.setTextColor(TFT_BLACK, TFT_GREEN);
                tft.fillScreen(TFT_GREEN);
                tft.println("WINNER X");
            }
            else if (winner == 2)
            {
                tft.setTextColor(TFT_BLACK, TFT_RED);
                tft.fillScreen(TFT_RED);
                tft.println("WINNER O");
            }
            else if (winner == 3)
            {
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.fillScreen(TFT_BLACK);
                tft.println("DRAW");
            }
            // should only be run once
            firstRun = false;
        }
    }
    // Host needs a delay to process the data (strangely)
    delay(5);
}

// Draws yellow rectangle where the positions is set to 15 (empty)
// return void
void drawEmptySpace()
{
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
        drawX(positionsXY[position][0], positionsXY[position][1], lengthX, TFT_BLACK);
    }
    else
    {
        Serial.println("player 2");
        tft.drawCircle(positionsXY[position][0] + 12, positionsXY[position][1] + 10, 14, TFT_BLACK);
    }
}

// Draws the all player based on the positionsSet (array[int]) variable.
// returns void
void drawAllPlayers() {
    drawEmptySpace();
    for (int i = 0; i <= quantity; i++)
    {
        if (positionsSet[i] == 1)
        {
            drawX(positionsXY[i][0], positionsXY[i][1], lengthX, TFT_BLACK);
        }
        if (positionsSet[i] == 2)
        {
            tft.drawCircle(positionsXY[i][0] + 12, positionsXY[i][1] + 10, 14, TFT_BLACK);
        }
    }
    determinePosition();
    checkForWinner(false);
    if (!winner) { checkForDraw(); }
    // Display current player
    tft.setCursor(5, 2, 2);
    tft.println("Spieler: " + String(player));
}

// Checks if somebody won based on the positionsSet (array[int]) variable.
// In addition if winner is set the client sends the player and positionsSet Variable to the host.
// Parameter:   buttonClicked (bool)
// returns void
void checkForWinner(bool buttonClicked) {
    // Check every possible move and set the winner
    score = positionsSet[0] + positionsSet[1] + positionsSet[2];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[3] + positionsSet[4] + positionsSet[5];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[6] + positionsSet[7] + positionsSet[8];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[0] + positionsSet[3] + positionsSet[6];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[1] + positionsSet[4] + positionsSet[7];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[2] + positionsSet[5] + positionsSet[8];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[0] + positionsSet[4] + positionsSet[8];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    score = positionsSet[2] + positionsSet[4] + positionsSet[6];
    if(score == 3 || score == 6) { winner = (score == 3) ? 1 : 2; }

    if (winner) {
        // Change the player for the last time so it can be grabt by the client.
        player = (player == 1) ? 2 : 1;
        // only send the variables to the host if it was a button click.
        if (!isServer && isOnline && buttonClicked) {
            httpPOSTRequest(serverGameSetPlayer, String(player));
            String tmpPositionsSet = convertToString();
            httpPOSTRequest(serverGameSetPositions, tmpPositionsSet);
        }
        fase++;
        firstRun = true;
    }
}

// When all moves are made go to next scene and display draw
// returns void
void checkForDraw() {
    for (int i = 0; i <= quantity; ++i)
    {
        if (positionsSet[i] == 15) { break; }
        else
        {
            if (i == quantity)
            {
                Serial.println(i);
                Serial.println(quantity);
                winner = 3;
                checkForWinner(false);
                return;
            }
        }
    }
}

// Draws a X with the given coordinates and color
// returns void
void drawX(int x, int y, int l, uint32_t color)
{
    tft.drawLine(x, y, x + l, y + l, color);
    tft.drawLine(x, y + l, x + l, y, color);
}

// Converts the post Request back to the Array.
// returns void
void convertToArray(String tempPositionsSet) {
    int str_len = tempPositionsSet.length() + 1; 
    char char_array[str_len];
    tempPositionsSet.toCharArray(char_array, str_len);

    const char* ptr = strtok(char_array, ",");
    byte i = 0;
    while (ptr) {
        positionsSet[i] = atol(ptr);
        ptr = strtok(NULL, ",");
        i++;
    }
}

// Converts the positionsSet array to a string.
// returns String
String convertToString() {
    String response = "";
    for (int i = 0; i <= quantity; i++)
    {
        if (i != quantity)
            response = response + String(positionsSet[i]) + ",";
        else
            response = response + String(positionsSet[i]);
    }
    return response;
}

// Setup the host for managing all request
// returns void
void setupServer() {

    // Setting the ESP as an access point
    Serial.print("Setting AP (Access Point)…");

    WiFi.softAP(ssid, password, 1, 1, 1); // ssid, password, channel, hidden SSID, max Connections

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    tft.println("Waiting for Player..");

    // wait for the client to connect
    while (WiFi.softAPgetStationNum() != 1) {
        delay(100);
    }

    // Configure all get and post requests
    server.on("/gameGetPositions", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String response = convertToString();
        request->send(200, "text/plain", response);
    });

    server.on("/gameGetPlayer", HTTP_GET, [](AsyncWebServerRequest *request){
        Serial.println(String(player));
        request->send(200, "text/plain", String(player));
    });

    server.on("/gamePostPositions", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncWebParameter *p = request->getParam(0);
        String tempPositionsSet = p->value().c_str();

        Serial.println("POST Positions: " + tempPositionsSet);

        convertToArray(tempPositionsSet);
        drawAllPlayers();
        request->send(200, "text/plain", "OK");
    });

    server.on("/gamePostPlayer", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncWebParameter *p = request->getParam(0);
        const char* tmpPlayer = p->value().c_str();

        player = String(tmpPlayer).toInt();
        Serial.println("POST PLAYER: " + String(player));
        request->send(200, "text/plain", "OK");
    });

    // start the server
    server.begin();
}

// Setup the client to connect to the host
// returns void
void setupClient() {
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    tft.println("Connecting..");
    // wait for the connection
    while(WiFi.status() != WL_CONNECTED) { 
        delay(100);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

// Displays if the host and the client are still connected
// returns void
void displayInternetConnectionStatus(bool connected) {
    tft.setTextColor(TFT_BLACK, post_yellow);
    tft.setTextSize(1);
    tft.setCursor(5, 225, 2);
    if (connected) {
        tft.println("Connected");
    } else {
        tft.println("Disconnected");
    }
}

// Handles the get requests
// returns String
String httpGETRequest(const char* serverName) {
    HTTPClient http;
        
    // Your IP address with path or Domain name with URL path 
    http.begin(serverName);
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
    
    String payload = "--"; 
    
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        payload = http.getString();
    } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    return payload;
}

// Handles the post requests
// returns String
String httpPOSTRequest(const char* serverName, String payload) {
    HTTPClient http; 
    
    http.begin(serverName);
    // needs a content header. If not set the host crashed!
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // prepare the payload and send the post.
    String preparedPayload = "test=" + payload;
    Serial.println(preparedPayload);
    int httpResponseCode = http.POST(preparedPayload);

    String response = "--"; 
    
    if(httpResponseCode>0){
    
        response = http.getString();    
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    
    } else{
    
        Serial.print("Error on sending POST Request: ");
        Serial.println(httpResponseCode);
    
    }
    
    http.end();
    return response;
}