
#include <SPI.h>
#include <TFT_eSPI.h>// Hardware-specific library
TFT_eSPI tft = TFT_eSPI();

#define post_yellow 0xFE60
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREY 0x8C51
#define WHITE 0xFFFF



unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500; //the value is a number of milliseconds used for the blinking player

int arrowPosition = 1;

void setup() {

    
    pinMode(0, INPUT);
    pinMode(35, INPUT);
    
  // put your setup code here, to run once:
    tft.init();
    tft.setRotation(0);
    
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE, BLACK);
    
    tft.setTextSize(1);
    
    tft.setCursor(35, 50, 2);
    tft.println("Create Game");

    tft.setCursor(35, 100, 2);
    tft.println("Join Game");
    
    tft.setCursor(35, 150, 2);
    tft.println("Local Game");

    tft.setCursor(10, 52, 1);
    tft.println("->");

    
     startMillis = millis(); //initial start time
}

void loop() {
  if (digitalRead(0) == 0){
      if(arrowPosition < 3){
        int position = (52 * 1) * arrowPosition;
        int newPosition = (52 * 1) * (arrowPosition + 1);
        
        tft.setCursor(10, position, 1);
        tft.println("  ");

        tft.setCursor(10, newPosition, 1);
        tft.println("->");

        arrowPosition++;
      }else{
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

      if (digitalRead(35) == 0)
        { // button right
        
          if (arrowPosition == 1){
              //todo => Create Game
         }
         else if (arrowPosition == 2) {
              //todo => Join Game
         }
         else if (arrowPosition == 3) {
               //todo => Local Game
          }
        }

}
