#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// Function prototypes
void readEncoder();
void startGame();
void checkInput();
void gameOver();
void selectNextAction();
void correctInput();

// Pin definitions for inputs and outputs
#define serial_TX 2
#define serial_RX 3
const int BrakePin = A3;      // Brake button pin
const int AcceleratePin = A4; // Accelerate button pin
const int HonkPin = A5;       // Honk button pin
const int EncoderPinA = 5;    // Rotary encoder A pin
const int EncoderPinB = 6;    // Rotary encoder B pin

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
   mp3.playMp3Folder(1); // Welcome sound or background music
 } else {
   Serial.println("Unable to communicate with DFPlayer Mini.");
 }

 brakeButton.setDebounceTime(50);
 accelerateButton.setDebounceTime(50);
 honkButton.setDebounceTime(50);

 pinMode(EncoderPinA, INPUT_PULLUP);
 pinMode(EncoderPinB, INPUT_PULLUP);
 attachInterrupt(digitalPinToInterrupt(EncoderPinA), readEncoder, CHANGE);

 // Initialize the OLED display
 u8g2.begin();
}

void loop() {
 brakeButton.loop();
 accelerateButton.loop();
 honkButton.loop();

 if (!gameActive && honkButton.isPressed()) {
   startGame();
 }

 if (gameActive) {
   checkInput();

   if (millis() - gameStartTime >= roundTime) {
     gameOver();
   }
 }
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

void startGame() {
 gameActive = true;
 score = 0;
 gameStartTime = millis(); // Initialize game start time

 // Display "Honk to start" or similar start game message
 u8g2.clearBuffer(); // Clear the internal memory
 u8g2.setFont(u8g2_font_6x10_tr);
 u8g2.drawStr(0, 30, "Start Game!");
 u8g2.sendBuffer(); // Transfer internal memory to the display
 delay(2000); // Give some delay before the game starts

 selectNextAction();
}

void gameOver() {
 gameActive = false;
 Serial.print("Game Over! Your score: ");
 Serial.println(score);

 // Display game over message
 u8g2.clearBuffer();
 u8g2.setFont(u8g2_font_6x10_tr);
 u8g2.drawStr(0, 15, "Game Over!");
 u8g2.setCursor(0, 30);
 u8g2.print("Score: ");
 u8g2.print(score);
 u8g2.sendBuffer();
 delay(3000); // Show game over screen for a bit before clearing
}

void selectNextAction() {
 // Fixed randomness range
 currentAction = static_cast<GameAction>(random(PRESS_BRAKE, PRESS_HONK + 1));
 gameStartTime = millis(); // Reset the start time for the action
 score++;

 // Display the action
 u8g2.clearBuffer();
 u8g2.setFont(u8g2_font_6x10_tr);
 u8g2.setCursor(0, 15);
 
switch (currentAction) {
    case PRESS_BRAKE: u8g2.drawStr(0, 15, "Press Brake!"); break;
    case PRESS_ACCELERATE: u8g2.drawStr(0, 15, "Accelerate!"); break;
    case STEER_LEFT: u8g2.drawStr(0, 15, "Steer Left!"); break;
    case STEER_RIGHT: u8g2.drawStr(0, 15, "Steer Right!"); break;
    case PRESS_HONK: u8g2.drawStr(0, 15, "Honk!"); break;
    default: break;
}
u8g2.setCursor(0, 30);
u8g2.print("Score: ");
u8g2.print(score);
u8g2.sendBuffer(); // Transfer internal memory to the display

// Introducing a slight delay helps with debouncing and gives the player time to react
delay(500);
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
     // Assuming that left turn decreases encoderPos
     if (lastEncoderPos > encoderPos) correctInput();
     break;
   case STEER_RIGHT:
     // Assuming that right turn increases encoderPos
     if (lastEncoderPos < encoderPos) correctInput();
     break;
   case PRESS_HONK:
     if (honkButton.isPressed()) correctInput();
     break;
   default:
     break;
 }

 // Update the last encoder position for the next check
 lastEncoderPos = encoderPos;
}

void correctInput() {
 Serial.println("Correct! Score: " + String(score));
 // This is where you can increment the score, play a sound, etc.
 // Score is already incremented in selectNextAction()
 selectNextAction(); // Move to the next action
}
