/*
  Tennis.cpp - Library for instances of LED strip tennis game.
  Charlie Pethica - May 2022
*/
#include "Arduino.h"
#include "Tennis.h"
#include "FastLED.h"
Tennis::Tennis(int playerBtnPin[], int playerLedPin[], const uint8_t dataPin, unsigned int numLeds)
{
	pinMode(playerBtnPin[0], INPUT_PULLUP);   // PINs for buttons
	pinMode(playerBtnPin[1], INPUT_PULLUP);
	_dataPin = dataPin;
	const unsigned int _numLeds = numLeds;
	_playerBtnPin = playerBtnPin;
	_playerLedPin = playerLedPin;

	// dataPin must be defined before runtime so define which one is used here...
	switch(_dataPin){
        case 16:
            FastLED.addLeds<WS2812B, 16, GRB>(_leds, _numLeds);
            break;
        case 18:
            FastLED.addLeds<WS2812B, 18, GRB>(_leds, _numLeds);
            break;
        default:
      		Serial.println("Unsupported Pin");
      		break;
    }

}

void Tennis::test1(){
    FastLED.setBrightness(50);
    fill_rainbow(_leds, 120, 150, 7); 
    FastLED.show(); 
}
void Tennis::test2(){
    FastLED.setBrightness(50);
    fill_rainbow(_leds, 120, 100, 30); 
    FastLED.show(); 
}

///
// DELAYS/WHILES !!!
///
// Something here causes glitches with 2 instances of class - can this section be removed???
void Tennis::menu(){
  // implement later	
   digitalWrite(_playerLedPin[0], HIGH);  // activate both LEDs
   digitalWrite(_playerLedPin[1], HIGH);

  GeneratePlayField(maxBright);         // show play-field
  FastLED.show();

  // set brightness (button player 0)
  if (buttonBounce(0, 5) == true)
  {
    maxBright += 50;
    if (maxBright > 255)
    {
      maxBright = 55;
    }
    GeneratePlayField(maxBright);
    FastLED.show();

    while (buttonBounce(0, 5) == true) {}   // wait for button release
  }

  // start game (button player 1)
  if (buttonBounce(1, 5) == true)
  {
    digitalWrite(_playerLedPin[0], LOW);     // deactivate both LEDs
    digitalWrite(_playerLedPin[1], LOW);
    _activeGame = true;

    while (buttonBounce(1, 5) == true) {}   // wait for button release

    game();
  }
}

// function to debounce button (true == pressed, false == not pressed)
boolean Tennis::buttonBounce(byte button, byte bounceTime) // bounce the button
{
  boolean result = false;

  if (digitalRead(_playerBtnPin[button]) == LOW)
  {
    delay (bounceTime);
    if (digitalRead(_playerBtnPin[button]) == LOW)
    {
      result = true;
    }
  }
  return result;
}

void Tennis::game()
{
  if (_activeGame)
  {

    _gameSpeed = gameSpeedMin;           // set starting game speed
    _ballSpeed = _gameSpeed;              // set starting ball speed
    memset(_playerButtonPressed, -1, sizeof(_playerButtonPressed)); // clear keypress

    GeneratePlayField(_scoreDimBright);  // show gamefield with dimmed score
    FastLED.show();

    InitializePlayers();  // set the player-settings -> wait for keypress to start game
    Serial.println("players initialized");
    GameLoop();           // main loop: move ball -> ball left gamefield? -> check keypress -> check if keypress in endzone -> change direction
    Serial.println("in game loop...");
    CheckScore();         // check who made score and show it

    CheckWinner();        // check if we have a winner
  }
}

///
// DELAYS/WHILES !!!
///
void Tennis::InitializePlayers()
{

  if (_playerStart == 0)   // initialize for player 0
  {
    _ballDir = 1;          // set ball direction
    _ballPos = 0;          // set startposition of ball

    digitalWrite(_playerLedPin[0], HIGH);              // activate LED
    while (digitalRead(_playerBtnPin[0]) == HIGH) {} // wait for button
    digitalWrite(_playerLedPin[0], LOW);               // deactivate LED
  }
  else                        // initialize for player 1
  {
    _ballDir = -1;             // set ball direction
    _ballPos = NUM_LEDS - 1;   // set startposition of ball

    digitalWrite(_playerLedPin[1], HIGH);              // activate LED
    while (digitalRead(_playerBtnPin[1]) == HIGH) {} // wait for button
    digitalWrite(_playerLedPin[1], LOW);               // deactivate LED
  }
}

///
// DELAYS/WHILES !!!
///
void Tennis::GameLoop()
{
  while (true)    // loop, exit with break when one player made a score
  {

    if (millis() - _previousMoveMillis > _ballSpeed)  // time to move ball
    {
      _previousMoveMillis = millis();

      GeneratePlayField(_scoreDimBright);

      _ballPos += _ballDir;
      if (_ballPos < 0 || _ballPos >= NUM_LEDS) // ball left endzone?
      {
        break;                // leave loop -> one player made a score
      }
      _leds[_ballPos] = CHSV(0, 0, maxBright);    // generate ball (white)
      FastLED.show();
    }

    CheckButtons();     // check keypress

    // fix positions of keypress for testing
    // if (ballPos == 3) playerButtonPressed[0] = 3;
    // if (ballPos == 59) playerButtonPressed[1] = 59;

    CheckButtonPressedPosition();

  } // end of while-loop
}


// *** check if buttons pressed
void Tennis::CheckButtons()
{
  for (int i = 0; i < 2; i++)
  {
    // player pressed button?
    if (_playerButtonPressed[i] == -1 && digitalRead(_playerBtnPin[i]) == LOW && (_ballDir + 1) / 2 == i)
      // (ballDir + 1) / 2 == i  -->  TRUE, when:
      // ballDir == -1  AND  i = 0  -->  player 0 is active player
      // ballDir == +1  AND  i = 1  -->  player 1 is active player
      // only the button-press of the active player is stored
    {
      _playerButtonPressed[i] = _ballPos;   //store position of pressed button
      _previousButtonColor = _ballPos;
      _previousButtonColor = playerColor[i];
      _previousButtonMillis = millis(); // store time when button was pressed
    }
  }
}


// *** check, if button was pressed when ball was in endzone and if so, change direction of ball
void Tennis::CheckButtonPressedPosition()
{
  if (_ballDir == -1 && _playerButtonPressed[0] <= endZoneSize - 1 && _playerButtonPressed[0] != -1)
  {
    ChangeDirection();
  }

  if (_ballDir == +1 && _playerButtonPressed[1] >= NUM_LEDS - endZoneSize)
  {
    ChangeDirection();
  }
}


void Tennis::ChangeDirection()
{
  _ballDir *= -1;
  _gameSpeed -= gameSpeedStep;
  _ballSpeed = _gameSpeed;
  if (_ballPos == 0 || _ballPos == NUM_LEDS - 1)  // triggered on first or last segment
  {
    _ballSpeed -= ballBoost0;      // Super-Boost
  }

  if (_ballPos == 1 || _ballPos == NUM_LEDS - 2)  // triggered on second or forelast segment
  {
    _ballSpeed -= ballBoost1;      // Boost
  }

  _ballSpeed = max(_ballSpeed, ballSpeedMax);                     // limit the maximum ballspeed
  memset(_playerButtonPressed, -1, sizeof(_playerButtonPressed)); // clear keypress
}

///
// DELAYS/WHILES !!!
///
void Tennis::CheckScore()
{
  _previousButtonPos = -1;     // clear last ball-position at button-press

  // check who made score
  if (_ballPos < 0)            // player1 made the score
  {
    _playerScore[1] += 1;      // new score for player1

    GeneratePlayField(maxBright);   // show new score full bright
    BlinkNewScore(NUM_LEDS / 2 - 1 + _playerScore[1], playerColor[1]); // blink last score

    _playerStart = 1;          // define next player to start (player, who made the point)
  }
  else                        // player0 made the score
  {
    _playerScore[0] += 1;      // new score for player0

    GeneratePlayField(maxBright);   // show new score full bright
    BlinkNewScore(NUM_LEDS / 2 - _playerScore[0], playerColor[0]); // blink last score

    _playerStart = 0;          // define next player to start (player, who made the point)
  }

  GeneratePlayField(maxBright);     // show new score full bright
  FastLED.show();

  delay(1000);
}


void Tennis::CheckWinner()
{
  // check if we have a winner
  if (_playerScore[0] >= winRounds || _playerScore[1] >= winRounds)
  { // we have a winner!

    _activeGame = false;

    FastLED.clear();
    Rainbow(_playerScore[0] > _playerScore[1]); // TRUE if player 0 won; FALSE when player 1 won

    memset(_playerScore, 0, sizeof(_playerScore));  // reset all scores

    _playerStart = abs(_playerStart - 1);   // next game starts looser

  }
}

void Tennis::GeneratePlayField(byte bright)
{
  FastLED.clear();        // clear all
  GenerateEndZone();      // generate endzone
  GenerateScore(bright);  // generate actual score
  GenerateLastHit();      // generate mark of position of last button-press
}

void Tennis::GenerateEndZone()
{
  for (int i = 0; i < endZoneSize; i++)
  {
    _leds[i] = CHSV(endZoneColor, 255, maxBright);
    _leds[NUM_LEDS - 1 - i] = CHSV(endZoneColor, 255, maxBright);
  }
}


void Tennis::GenerateScore(int bright)
{
  int i;

  // Player 0
  for (i = 0; i < _playerScore[0]; i++)
  {
    _leds[NUM_LEDS / 2 - 1 - i] = CHSV(playerColor[0], 255, bright);
  }

  // Player 1
  for (i = 0; i < _playerScore[1]; i++)
  {
    _leds[NUM_LEDS / 2 + i] = CHSV(playerColor[1], 255, bright);
  }
}


void Tennis::GenerateLastHit()
{
  if (_previousButtonPos != -1 && _previousButtonMillis + 500 > millis())
  {
    _leds[_previousButtonPos] = CHSV(_previousButtonColor, 255, _previousButtonBright);
  }
}

///
// DELAYS/WHILES !!!
///
void Tennis::BlinkNewScore(int pos, byte color)
{
  for (int i = 1; i <= 4; i++)
  {
    _leds[pos] = CHSV(color, 255, (i % 2) * maxBright);  // blink LED 2 times (1-0-1-0)
    FastLED.show();
    delay(300);
  }
}

///
// DELAYS/WHILES !!! 
///
void Tennis::Rainbow(boolean won)
{
  for (int k = 0; k < 3; k++)   // 3 rounds rainbow
  {
    for (int j = 0; j <= 255; j++)
    {
      for (int i = 0; i < NUM_LEDS / 2; i++)
      {
        if (won == true)    // player 0 won --> Rainbow left
        {
          _leds[i] = CHSV(((i * 256 / NUM_LEDS) + j) % 256, 255, maxBright);
        }
        else                // player 1 won --> Rainbow right
        {
          _leds[NUM_LEDS - i - 1] = CHSV(((i * 256 / NUM_LEDS) + j) % 256, 255, maxBright);
        }
      }
      FastLED.show();
      delay(7);
    }
  }
}