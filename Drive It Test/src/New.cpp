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

  u8g2.begin();
  startGame();
}

void loop() {
  brakeButton.loop();
  accelerateButton.loop();
  honkButton.loop();

  if (gameActive && !waitingForInput) {
    selectNextAction();
    waitingForInput = true;
    actionStartTime = millis();
  } else if (waitingForInput && millis() - actionStartTime <= inputWaitTime) {
    checkForInput();
  } else if (waitingForInput && millis() - actionStartTime > inputWaitTime) {
    waitingForInput = false; // Timeout, proceed without input
  }

  if (!gameActive && millis() - gameStartTime >= roundTime) {
    gameOver();
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
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.print("Start Game!");
  } while (u8g2.nextPage());
  delay(500); // Short delay before starting the game
  u8g2.clearDisplay();
}

void gameOver() {
  gameActive = false;
  Serial.print("Game Over! Your score: ");
  Serial.println(score);
  delay(1000);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(0, 15);
    u8g2.print("Game Over!");
    u8g2.setCursor(0, 30);
    u8g2.print("Score: ");
    u8g2.print(score);
  } while (u8g2.nextPage());
  delay(1000);
}

void selectNextAction() {
  if (!gameActive) return;
  int randomNumber = random(1, 6);
  gameStartTime = millis();
  score++;
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(0, 15);
    switch(randomNumber)
     {
        case 1: 
            currentAction = PRESS_HONK; 
            u8g2.print("Honk!"); 
            break;
        case 2: 
            currentAction = STEER_LEFT; 
            u8g2.print("Steer Left!"); 
            break;
        case 3: 
            currentAction = STEER_RIGHT; 
            u8g2.print("Steer Right!"); 
            break;
        case 4: 
            currentAction = PRESS_BRAKE; 
            u8g2.print("Press Brake!"); 
            break;
        case 5: 
            currentAction = PRESS_ACCELERATE; 
            u8g2.print("Accelerate!"); 
            break;
    }
    u8g2.setCursor(0, 30);
    u8g2.print("Score: ");
    u8g2.print(score);
  } while (u8g2.nextPage());
}

void checkForInput() {
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
    lastEncoderPos = encoderPos; // Update the last encoder position after checking
}

void correctInput() {
    Serial.println("Correct! Score: " + String(score));
    waitingForInput = false; // Reset to allow selecting the next action
    // Optionally, play a sound or give visual feedback for correct input
    // mp3.playMp3Folder(2); // Assuming file 2 is the correct input sound
    delay(500); // Short delay to acknowledge correct input before moving on
}

void attachInterrupts() {
    detachInterrupt(digitalPinToInterrupt(EncoderPinA));
    attachInterrupt(digitalPinToInterrupt(EncoderPinA), readEncoder, CHANGE);
}

// Optional: Implement a method to detach interrupts if you need to disable encoder readings at any point
void detachInterrupts() {
    detachInterrupt(digitalPinToInterrupt(EncoderPinA));
}
