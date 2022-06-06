
#define FASTLED_INTERNAL       // Disable version number message in FastLED library (looks like an error)
#include <FastLED.h>
#include <Tennis.h>

// LED Strips:
uint8_t gHue = 0;  // for strip testing

// buttons 13,14,27,15     ---- 13, 27 at far end
// WS2812 data 16,18
int playerBtnPin1[] =   {14, 15}; // Pins for buttons
int playerLedPin1[] =   {5, 23}; // Pins for LEDs in buttons

int playerBtnPin2[] =   {27, 13}; // Pins for buttons
int playerLedPin2[] =   {5, 23}; // Pins for LEDs in buttons


//Tennis(int playerBtnPin[], int playerLedPin[], const uint8_t dataPin, unsigned int numLeds)
Tennis Game1(playerBtnPin1, playerLedPin1, 16, 120);
Tennis Game2(playerBtnPin2, playerLedPin2, 18, 120);

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

}

void loop() {

     Game1.game();
     //Game2.game();    

}
