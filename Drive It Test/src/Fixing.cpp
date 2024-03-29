#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

// Screen initialization
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 9, 8);

// Pin definitions
#define serial_TX 2
#define serial_RX 3
const int BrakePin = A3;
const int AcceleratePin = A4;
const int HonkPin = A5;
const int EncoderPinA = 5;
const int EncoderPinB = 6;

// Encoder position tracking
volatile int encoderPos = 0;
int lastEncoderPos = 0;

// Game control variables
unsigned long gameStartTime;
const long roundTime = 5000; // 5 seconds per round
bool gameActive = false;
int score = 0;
bool waitingForInput = false;
unsigned long actionStartTime = 0;
const long inputWaitTime = 2000; // 2 seconds to respond to an action

// Buttons
ezButton brakeButton(BrakePin);
ezButton accelerateButton(AcceleratePin);
ezButton honkButton(HonkPin);

// DFPlayer Mini
SoftwareSerial softwareSerial(serial_RX, serial_TX);
DFRobotDFPlayerMini mp3;

// Game action enumeration
enum GameAction { PRESS_BRAKE, PRESS_ACCELERATE, STEER_LEFT, STEER_RIGHT, PRESS_HONK, NONE };
GameAction currentAction = NONE;

void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);
  if (mp3.begin(softwareSerial)) {
    Serial.println("DFPlayer Mini is online.");
    mp3.volume(30);
    mp3.playMp3Folder(1); // Welcome sound
  } else {
    Serial.println("Unable to communicate with DFPlayer Mini.");
  }

  brakeButton.setDebounceTime(50);
  accelerateButton.setDebounceTime(50);
  honkButton.setDebounceTime(50);

  pinMode(EncoderPinA, INPUT_PULLUP);
  pinMode(EncoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EncoderPinA), readEncoder, CHANGE);

  u8g2.begin();
  startGame();
}

void loop() {
  brakeButton.loop();
  accelerateButton.loop();
  honkButton.loop();

  if (gameActive) {
    unsigned long currentMillis = millis();

    if (!waitingForInput) {
      selectNextAction();
      actionStartTime = currentMillis;
      waitingForInput = true;
    } else if (waitingForInput && currentMillis - actionStartTime <= inputWaitTime) {
      checkForInput();
    } else if (waitingForInput && currentMillis - actionStartTime > inputWaitTime) {
      Serial.println("Too slow! Try again!");
      waitingForInput = false; // Timeout, proceed without input
      gameActive = false; // End game on timeout for simplicity, or implement a penalty system.
    }

    if (gameActive && currentMillis - gameStartTime >= roundTime) {
      gameOver();
    }
  }
}

void readEncoder() {
  int a = digitalRead(EncoderPinA);
  int b = digitalRead(EncoderPinB);
  encoderPos += (a != b) ? 1 : -1; // Simplified to avoid potential issues with reading logic.
}

void startGame() {
  gameActive = true;
  gameStartTime = millis(); // Record start time of the game
  score = 0;
  displayMessage("Start Game!", 0, 15);
  delay(1000); // Short delay before starting the game
}

void gameOver() {
  gameActive = false;
  Serial.print("Game Over! Your score: ");
  Serial.println(score);
  displayMessage("Game Over! Score: " + String(score), 0, 15);
  mp3.playMp3Folder(3); // Assuming file 3 is the game over sound
  delay(3000); // Give time for the game over message and sound
  startGame(); // Restart the game automatically or prompt for restart
}

void selectNextAction() {
  if (!gameActive) return;
  int randomNumber = random(1, 6); // Select a random action
  currentAction = static_cast<GameAction>(randomNumber - 1);
  score++;
  displayActionMessage(currentAction);
}

void checkForInput() {
  bool correctInput = false;

  switch(currentAction) {
    case PRESS_BRAKE: correctInput = brakeButton.isPressed(); break;
    case PRESS_ACCELERATE: correctInput = accelerateButton.isPressed(); break;
    case STEER_LEFT: correctInput = (lastEncoderPos > encoderPos); break;
    case STEER_RIGHT: correctInput = (lastEncoderPos < encoderPos); break;
    case PRESS_HONK: correctInput = honkButton.isPressed(); break;
    default: break;
  }

  if (correctInput) {
    Serial.println("Correct! Score: " + String(score));
    mp3.playMp3Folder(2); // Correct input sound
    waitingForInput = false; // Reset to allow selecting the next action
    delay(500); // Short delay to acknowledge correct input before moving on
  }
  lastEncoderPos = encoderPos; // Update the last encoder position after checking
}

void displayMessage(String message, int x, int y) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(x, y, message.c_str());
  u8g2.sendBuffer();
}

void displayActionMessage(GameAction action) {
  String message;
  switch(action) {
    case PRESS_BRAKE: message = "Press Brake!"; break;
    case PRESS_ACCELERATE: message = "Accelerate!"; break;
    case STEER_LEFT: message = "Steer Left!"; break;
    case STEER_RIGHT: message = "Steer Right!"; break;
    case PRESS_HONK: message = "Honk!"; break;
    default: message = "Unknown Action"; break;
  }
  displayMessage(message + " Score: " + String(score), 0, 15);
}
