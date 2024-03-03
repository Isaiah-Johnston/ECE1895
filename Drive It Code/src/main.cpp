#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>
#include <string.h>

// static pins
#define serial_TX   2
#define serial_RX   3
#define run_sw      4
#define encoder_CLK 5
#define encoder_DT  6
#define encoder_SW  7
#define brake_sw   28
#define accel_sw   27

// static vars
#define clockwise   0
#define cclockwise  1

// global vars
uint32_t encoder_direction;
int32_t  CLK_count;
uint8_t  curr_CLK_state;
uint8_t  prev_CLK_state;

// def objects
DFRobotDFPlayerMini mp3;
SoftwareSerial      softwareSerial(serial_RX, serial_TX);
ezButton            encoder_button(encoder_SW);

void setup() {
  // def I/O type
  pinMode(encoder_CLK, INPUT);
  pinMode(encoder_DT , INPUT);
  pinMode(run_sw     , INPUT);

  // init USB for debugging 
  // init serial port for transmission
  Serial.begin(9600);
  softwareSerial.begin(9600);

  Serial.println("HELLO WORLD");

// ROTARY ENCODER
{
  // set debouncing period to 50ms
  encoder_button.setDebounceTime(100);
  // init previous encoder state
  prev_CLK_state = digitalRead(encoder_CLK);
}

// DFplayerMini MP3 PLAYER
{
  // start comms with device
  if(mp3.begin(softwareSerial)) {
    Serial.println("Connection to DFplayerMini successful!");

    // set volume 0-30
    mp3.volume(30);
    
    // play 0001.mp3 in "mp3" folder on SD
    mp3.playMp3Folder(1);
  }
  else 
    Serial.println("Connection to DFplayerMini failed!");
}

}

// Start the game, "honk to start" on display maybe. after button press enter loop

void loop() {
  // Select a random number 1-5:
  // 1: honk 
  // 2: turn left
  // 3: turn right
  // 4: brake
  // 5: accel

  int randomNumber = random(1, 6);

  switch (randomNumber) {
    case 1:
      // Play sound for honk button press command
      playDFPlayerSound(1);  // Adjust sound number as needed
      Serial.println("Case 1: Honk command sound. Waiting for button press.");
      
      // Wait for the rotary button to be pressed
      while (!encoder_button.isPressed()) {
        encoder_button.loop();
        
        // Check if another action is taken
        if (digitalRead(run_sw) || digitalRead(encoder_SW) || (encoder_direction == cclockwise) || (encoder_direction == clockwise)) {
          // Play an error sound
          playDFPlayerSound(7);  // Adjust sound number as needed for an error sound
          Serial.println("Incorrect action taken. Exiting loop.");
          return;  // Exit the loop
        }
      }

      // Play honk sound
      playDFPlayerSound(6);  // Adjust sound number as needed
      break;

    case 2:
      // Play the sound for left turn command
      playDFPlayerSound(2);  // Adjust sound number as needed
      Serial.println("Case 2: Left turn command sound. Waiting for counter-clockwise rotation.");
      while (encoder_direction != cclockwise) {
        encoder_button.loop();
        
        // Check if another action is taken
        if (digitalRead(run_sw) || digitalRead(encoder_SW) || (encoder_direction == cclockwise) || (encoder_direction == clockwise)) {
          // Play an error sound
          playDFPlayerSound(7);  // Adjust sound number as needed for an error sound
          Serial.println("Incorrect action taken. Exiting loop.");
          return;  // Exit the loop
        }
      }
      break;

    case 3:
      // Play the correct sound for right turn command
      playDFPlayerSound(3);  // Adjust sound number as needed
      Serial.println("Case 3: Right turn command sound. Waiting for clockwise rotation.");
      while (encoder_direction != clockwise) {
        encoder_button.loop();
        
        // Check if another action is taken
        if (digitalRead(run_sw) || digitalRead(encoder_SW) || (encoder_direction == cclockwise) || (encoder_direction == clockwise)) {
          // Play an error sound
          playDFPlayerSound(7);  // Adjust sound number as needed for an error sound
          Serial.println("Incorrect action taken. Exiting loop.");
          return;  // Exit the loop
        }
      }
      break;

    case 4:
      // Play the correct sound for brake command
      playDFPlayerSound(4);  // Adjust sound number as needed
      Serial.println("Case 4: Brake command sound. Waiting for brake limit switch.");
      while (!digitalRead(brake_sw)) {
        encoder_button.loop();
        
        // Check if another action is taken
        if (digitalRead(run_sw) || digitalRead(encoder_SW) || (encoder_direction == cclockwise) || (encoder_direction == clockwise)) {
          // Play an error sound
          playDFPlayerSound(7);  // Adjust sound number as needed for an error sound
          Serial.println("Incorrect action taken. Exiting loop.");
          return;  // Exit the loop
        }
      }
      break;

    case 5:
      // Play the correct sound for acceleration command
      playDFPlayerSound(5);  // Adjust sound number as needed
      Serial.println("Case 5: Accelerator command sound. Waiting for accelerator limit switch.");
      while (digitalRead(accel_sw)) {
        encoder_button.loop();
        
        // Check if another action is taken
        if (digitalRead(run_sw) || digitalRead(encoder_SW) || (encoder_direction == cclockwise) || (encoder_direction == clockwise)) {
          // Play an error sound
          playDFPlayerSound(7);  // Adjust sound number as needed for an error sound
          Serial.println("Incorrect action taken. Exiting loop.");
          return;  // Exit the loop
        }
      }
      break;
  }
}

void playDFPlayerSound(int soundNumber) {
  // Play the specified sound on the DFPlayer Mini
  mp3.playMp3Folder(soundNumber);
  delay(500);  // Adjust delay if needed
}