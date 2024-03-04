#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

// Initialize buttons using ezButton library
ezButton brakeButton(BrakePin);
ezButton accelerateButton(AcceleratePin);
ezButton honkButton(HonkPin);

// Initialize DFPlayer Mini on a software serial port
SoftwareSerial softwareSerial(serial_RX, serial_TX); // RX, TX
DFRobotDFPlayerMini mp3;

// Initialize OLED Display
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  // Start the game
  startGame();

  // Initialize OLED display
  if (!display.begin(SSD1306_I2C, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display(); // Display splash screen
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Update button states
  brakeButton.loop();
  accelerateButton.loop();
  honkButton.loop();

  // Game logic here
  checkInput(); // Check the player's input

  // Display score on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Score: " + String(score));
  display.display();
  
  // Check if time for the current action has expired
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

  // Display updated score immediately after a correct input
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Score: " + String(score));
  display.display();
}
