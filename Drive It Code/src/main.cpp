#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Pin definitions for inputs and outputs
#define serial_TX 2
#define serial_RX 3
const int BrakePin = 28;      // Brake button pin
const int AcceleratePin = 27; // Accelerate button pin
const int HonkPin = 11;       // Honk button pin
const int EncoderPinA = 5;    // Rotary encoder A pin
const int EncoderPinB = 6;    // Rotary encoder B pin
const int feedbackPin = 12;   // Feedback pin (e.g., an LED or a buzzer for game feedback)

// Encoder position tracking
volatile int encoderPos = 0;
int lastEncoderPos = 0;

// Game control variables
unsigned long gameStartTime;
const long roundTime = 5000; // Time per round in milliseconds (5 seconds)
bool gameActive = false;
int score = 0;

// OLED Display
U8G2_SSD1309_128X64_NONAME0_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Initialize buttons using ezButton library
ezButton brakeButton(BrakePin);
ezButton accelerateButton(AcceleratePin);
ezButton honkButton(HonkPin);

// Initialize DFPlayer Mini on a software serial port
SoftwareSerial softwareSerial(serial_RX, serial_TX); // RX, TX
DFRobotDFPlayerMini mp3;

// Game action enumeration for clarity
enum GameAction { PRESS_BRAKE, PRESS_ACCELERATE, STEER_LEFT, STEER_RIGHT, PRESS_HONK, NONE };
GameAction currentAction = NONE;

void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600); // Start the software serial for DFPlayer Mini

  // Initialize the DFPlayer Mini
  if (mp3.begin(softwareSerial)) {
    Serial.println("DFPlayer Mini is online.");
    mp3.volume(30); // Set the volume value (0~30)
    mp3.playMp3Folder(1); // Play the first mp3 in the "mp3" folder
  } else {
    Serial.println("Unable to communicate with DFPlayer Mini.");
  }

  // Initialize button pins
  brakeButton.setDebounceTime(50); // set debounce time to 50 milliseconds
  accelerateButton.setDebounceTime(50);
  honkButton.setDebounceTime(50);

  // Initialize encoder pins
  pinMode(EncoderPinA, INPUT_PULLUP);
  pinMode(EncoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EncoderPinA), readEncoder, CHANGE); // Attach interrupt for encoder

  // Initialize feedback pin
  pinMode(feedbackPin, OUTPUT);

  // Initialize U8g2 OLED display
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 20, "Initialization");
  u8g2.drawStr(0, 40, "Successful");
  u8g2.sendBuffer();
  delay(2000);
  u8g2.clearBuffer();
  u8g2.sendBuffer();

  // Start the game
  startGame();
}

void loop() {
  // Update button states
  brakeButton.loop();
  accelerateButton.loop();
  honkButton.loop();

  // Game logic here
  checkInput(); // Check the player's input

  // Check if time for the current action has expired
  if (millis() - gameStartTime >= roundTime) {
    gameOver();
  }

  // Display score on OLED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.setCursor(0, 20);
  u8g2.print("Score: ");
  u8g2.print(score);
  u8g2.sendBuffer();
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
}

void gameOver() {
  gameActive = false;
  Serial.print("Game Over! Your score: ");
  Serial.println(score);

  // Signal game over
  digitalWrite(feedbackPin, HIGH);
  delay(1000);
  digitalWrite(feedbackPin, LOW);

  // Optionally, restart game or wait for a restart command
}

void selectNextAction() {
  currentAction = static_cast<GameAction>(random(PRESS_BRAKE, PRESS_HONK + 1));
  gameStartTime = millis(); // Reset round start time
  score++;
  
  Serial.print("Score: ");
  Serial.print(score);
  Serial.print(" - ");
  
  switch(currentAction) {
    case PRESS_BRAKE: Serial.println("Press Brake!"); mp3.playMp3Folder(4); break; // Example: mp3 file 4 for brake
    case PRESS_ACCELERATE: Serial.println("Press Accelerate!"); mp3.playMp3Folder(5); break; // Example: mp3 file 5 for accelerate
    case STEER_LEFT: Serial.println("Steer Left!"); mp3.playMp3Folder(2); lastEncoderPos = encoderPos; break; // Example: mp3 file 2 for left
    case STEER_RIGHT: Serial.println("Steer Right!"); mp3.playMp3Folder(3); lastEncoderPos = encoderPos; break; // Example: mp3 file 3 for right
    case PRESS_HONK: Serial.println("Press Honk!"); mp3.playMp3Folder(1); break; // Example: mp3 file 1 for honk
    default: break;
  }
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
      // Check for left rotation from the encoder
      if (lastEncoderPos > encoderPos) correctInput();
      break;
    case STEER_RIGHT:
      // Check for right rotation from the encoder
      if (lastEncoderPos < encoderPos) correctInput();
      break;
    case PRESS_HONK:
      if (honkButton.isPressed()) correctInput();
      break;
    default:
      break;
  }
  
  // Update last encoder position for next check
  lastEncoderPos = encoderPos;
}

void correctInput() {
  Serial.println("Correct! Score: " + String(score));
  selectNextAction(); // Move to the next round
}
