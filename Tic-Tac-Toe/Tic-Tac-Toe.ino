// #include <EEPROM.h> // Write Data to storage
#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "esp_wifi.h"
#include "ESPAsyncWebServer.h"
#include <HTTPClient.h>

// Colors
#define post_yellow 0xFE60
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREY 0x8C51
#define WHITE 0xFFFF

long colors[] = {0xFE60, 0x0000, 0x001F, 0xF800, 0x8C51}; // post_yellow, Black, Blue, Red, Grey

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

// millis part of blinking player
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500; //the value is a number of milliseconds used for the blinking player

// int storedColor;

bool firstRun = true;

// Set your access point network credentials
const char* ssid = "ESP32-TicTacToe";
const char* password = "ticTacToe";

const char* serverGameGetPlayer = "http://192.168.4.1/gameGetPlayer";
const char* serverGameGetPositions = "http://192.168.4.1/gameGetPositions";
const char* serverGameSetPlayer = "http://192.168.4.1/gamePostPlayer";
const char* serverGameSetPositions = "http://192.168.4.1/gamePostPositions";
bool isServer = true;
bool isOnline = true;

AsyncWebServer server(80);

void setup(void)
{
    // EEPROM.begin(10);
    // initialize Serial Output
    Serial.begin(115200);

    // storedColor = EEPROM.read(0);

    // Serial.println("Value: " + String(storedColor));

    // Set pin mode of the two inbuilt buttons
    pinMode(0, INPUT);
    pinMode(35, INPUT);

    // Init TFT
    tft.init();
    tft.setRotation(0);

    tft.setSwapBytes(true);

    initScreen();

    // tft.pushImage(0, 0, 135, 240, bootlogo);

    startMillis = millis(); //initial start time

    // EEPROM.write(0, storedColor);

    // EEPROM.commit();

    // setupServer();

}

void loop()
{
    //start screen
    if (fase == 0)
    {
        if (digitalRead(0) == 0)
        {
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
                int position = (52 * 1) * arrowPosition;
                int newPosition = (52 * 1) * (arrowPosition + 1);

                tft.setCursor(10, position, 1);
                tft.println("  ");

                tft.setCursor(10, newPosition, 1);
                tft.println("->");

                arrowPosition++;
            }
            else
            {
                int position = (52 * 1) * arrowPosition;
                arrowPosition = 1;
                int newPosition = (52 * 1) * (arrowPosition);

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
        currentMillis = millis(); //get the current "time" (actually the number of milliseconds since the program started)

        if(firstRun) {
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
            firstRun = false;
            delay(1000);
        }

        if (currentMillis - startMillis >= period) //test whether the period has elapsed
        {
            if (isOnline) {
                if (!isServer) {
                    player = httpGETRequest(serverGameGetPlayer).toInt();
                    Serial.println(String(player));
                    if (player > 1) {
                        convertToArray(httpGETRequest(serverGameGetPositions));
                        drawAllPlayers();
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

            // check if somebody won
            if (!winner)
            {
                checkForDraw();
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
            firstRun = false;
        }
    }
    // Host needs a delay to process the data (strangely)
    delay(5);
}

void initScreen() {
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE, BLACK);
    
    tft.setTextSize(1);
    
    tft.setCursor(35, 50, 2);
    tft.println("Host Game");

    tft.setCursor(35, 100, 2);
    tft.println("Join Game");
    
    tft.setCursor(35, 150, 2);
    tft.println("Local Game");

    tft.setCursor(10, 52, 1);
    tft.println("->");
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

void drawAllPlayers() {
    drawEmptySpace();
    for (int i = 0; i <= quantity; i++)
    {
        if (positionsSet[i] == 1)
        {
            drawX(positionsXY[i][0], positionsXY[i][1], lengthX, BLACK);
        }
        if (positionsSet[i] == 2)
        {
            tft.drawCircle(positionsXY[i][0] + 12, positionsXY[i][1] + 10, 14, BLACK);
        }
    }
    determinePosition();
    checkForWinner(false);
    if (!winner) { checkForDraw(); }
    // Display current player
    tft.setCursor(5, 2, 2);
    tft.println("Spieler: " + String(player));
}

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
        if (!isServer && isOnline && buttonClicked) {
            httpPOSTRequest(serverGameSetPlayer, String(player));
            String tmpPositionsSet = convertToString();
            httpPOSTRequest(serverGameSetPositions, tmpPositionsSet);
        }
        fase++;
        firstRun = true;
    }
}

void checkForDraw() {
    // When all moves are made go to next scene and display drawW
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
void drawX(int x, int y, int l, uint32_t color)
{
    tft.drawLine(x, y, x + l, y + l, color);
    tft.drawLine(x, y + l, x + l, y, color);
}

// Converts the post Request back to the Array.
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

void setupServer() {

    // Setting the ESP as an access point
    Serial.print("Setting AP (Access Point)…");

    WiFi.softAP(ssid, password, 1, 1, 1); // ssid, password, channel, hidden SSID, max Connections

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    tft.println("Waiting for Player..");

    while (WiFi.softAPgetStationNum() != 1) {
        delay(100);
    }

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

    server.begin();
}

void setupClient() {    
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    tft.println("Connecting..");
    while(WiFi.status() != WL_CONNECTED) { 
        delay(100);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

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

String httpPOSTRequest(const char* serverName, String payload) {
    HTTPClient http; 
    
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

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
