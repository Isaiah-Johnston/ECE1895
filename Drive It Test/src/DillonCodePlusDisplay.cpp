#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>
#include <U8g2lib.h>

// Pin definitions for inputs, outputs, and OLED display
#define serial_TX 2
#define serial_RX 3
const int BrakePin = 28;
const int AcceleratePin = 27;
const int HonkPin = 11;
const int EncoderPinA = 5;
const int EncoderPinB = 6;
const int feedbackPin = 12;

// OLED display setup using software SPI
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// Encoder position tracking
volatile int encoderPos = 0;
int lastEncoderPos = 0;

// Game control variables
unsigned long gameStartTime;
const long roundTime = 5000; // 5 seconds per round
bool gameActive = false;
int score = 0;

// Initialize buttons
ezButton brakeButton(BrakePin);
ezButton accelerateButton(AcceleratePin);
ezButton honkButton(HonkPin);

// Initialize DFPlayer Mini on a software serial port
SoftwareSerial softwareSerial(serial_RX, serial_TX); // RX, TX
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
    mp3.playMp3Folder(1);
  } else {
    Serial.println("Unable to communicate with DFPlayer Mini.");
  }

  brakeButton.setDebounceTime(50);
  accelerateButton.setDebounceTime(50);
  honkButton.setDebounceTime(50);

  pinMode(EncoderPinA, INPUT_PULLUP);
  pinMode(EncoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EncoderPinA), readEncoder, CHANGE);

  pinMode(feedbackPin, OUTPUT);

  // Initialize the OLED display
  u8g2.begin();

  // Start the game
  startGame();
}

void loop() {
  brakeButton.loop();
  accelerateButton.loop();
  honkButton.loop();

  checkInput();

  if (millis() - gameStartTime >= roundTime) {
    gameOver();
  }
}

void readEncoder() {
  if (digitalRead(EncoderPinA) == digitalRead(EncoderPinB)) {
    encoderPos++;
  } else {
    encoderPos--;
  }
}

void startGame() {
  gameActive = true;
  score = 0;
  selectNextAction();

  // Display start message
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(0, 30, "Start Game!");
  } while (u8g2.nextPage());
}

void gameOver() {
  gameActive = false;
  Serial.print("Game Over! Your score: ");
  Serial.println(score);

  digitalWrite(feedbackPin, HIGH);
  delay(1000);
  digitalWrite(feedbackPin, LOW);

  // Display game over message
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(0, 15, "Game Over!");
    u8g2.setCursor(0, 30);
    u8g2.print("Score: ");
    u8g2.print(score);
  } while (u8g2.nextPage());
}

void selectNextAction() {
  currentAction = static_cast<GameAction>(random(PRESS_BRAKE, PRESS_HONK + 1));
  gameStartTime = millis();
  score++;
  
  // Display the action
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(0, 15);
    switch(currentAction) {
      case PRESS_BRAKE: u8g2.print("Press Brake!"); break;
case PRESS_ACCELERATE: u8g2.print("Accelerate!"); break;
case STEER_LEFT: u8g2.print("Steer Left!"); break;
case STEER_RIGHT: u8g2.print("Steer Right!"); break;
case PRESS_HONK: u8g2.print("Honk!"); break;
default: break;
}
u8g2.setCursor(0, 30);
u8g2.print("Score: ");
u8g2.print(score);
} while (u8g2.nextPage());
}

void checkInput() {
if (!gameActive) return;

switch(currentAction) {
case PRESS_BRAKE:
if (brakeButton.isPressed()) correctInput();
break;
case PRESS_ACCELERATE:
if (accelerateButton.isPressed()) correctInput();
break;
case STEER_LEFT:
if (lastEncoderPos > encoderPos) correctInput();
break;
case STEER_RIGHT:
if (lastEncoderPos < encoderPos) correctInput();
break;
case PRESS_HONK:
if (honkButton.isPressed()) correctInput();
break;
default:
break;
}

// Update the last encoder position
lastEncoderPos = encoderPos;
}

void correctInput() {
Serial.println("Correct! Score: " + String(score));
selectNextAction(); // Move to the next round
}

void readEncoder() {
int a = digitalRead(EncoderPinA);
int b = digitalRead(EncoderPinB);
if (a != b) {
encoderPos++;
} else {
encoderPos--;
}
}
