/*
  Tennis.h - Library for instances of LED strip tennis game.
  Charlie Pethica - May 2022
*/

#ifndef Tennis_h
#define Tennis_h
#include "Arduino.h"
#include "FastLED.h"
class Tennis {
// define variables and function accessible from sketch here:
public:
  
  Tennis(int playerBtnPin[], int playerLedPin[], const uint8_t dataPin, unsigned int numLeds);          // class constructor - setup class from skectch
  
  // public variables within class
  int numLeds;
  int maxBright = 55; // max brightness
  int gameSpeedMin    =       50; // min game-speed
  int gameSpeedMax    =        5; // max game-speed
  int gameSpeedStep   =        1; // fasten up when change direction
  int ballSpeedMax    =        1; // max ball-speed
  int ballBoost0      =       25; // superboost on last position
  int ballBoost1      =       15; // boost on forelast position
  byte playerColor[2]  =  {0, 96}; // red & green
  int winRounds       =       10; // x winning rounds for winning game
  int endZoneSize     =        12; // size endzone
  int endZoneColor    =      160; // color endzone
  int NUM_LEDS        =      120; // number of leds per strip

  //public functions within class
  void test1();
  void test2();  
  void menu();    // menu and start game function
  boolean buttonBounce(byte button, byte bounceTime);
  void game();
  void InitializePlayers();
  void GameLoop();
  void CheckButtons();
  void CheckButtonPressedPosition();
  void ChangeDirection();
  void CheckScore();
  void CheckWinner();
  void GeneratePlayField(byte bright);
  void GenerateEndZone();
  void GenerateScore(int bright);
  void GenerateLastHit();
  void BlinkNewScore(int pos, byte color);
  void Rainbow(boolean won);
// variables and functions only used by class defined here:

private:
  
  int* _playerBtnPin;               // declaration of buttons variable within class only
  int* _playerLedPin;
  int _dataPin;

  // change to true for testing....
  boolean _activeGame = true;                 // true when game is active
  unsigned long _previousMoveMillis;           // time of last ball move
  unsigned long _previousButtonMillis;         // time of last button-press
  int _playerButtonPressed[2];                 // ball-position where button was pressed; „-1“ button not pressed
  int _previousButtonPos = -1;                 // position of last button-press
  byte _previousButtonColor;                   // color of field for last Button-press
  int _playerScore[2];                         // actual Score
  byte _playerStart = 0;                           // who starts game
  int _gameSpeed;                              // actual game-speed
  int _ballDir = 1;                            // direction, ball is moving (+/- 1)
  int _ballPos;                                // actual ball-position
  int _ballSpeed;                              // actual ball-speed (higher = slower)
  byte _previousButtonBright = maxBright / 2;  // bright of marked last position when button pressen
  byte _scoreDimBright       = maxBright / 4;  // bright of dimmed score
  CRGB _leds[120];
  boolean _scoreFlag = false;                           // use in place of game while(true) loop

};
#endif
