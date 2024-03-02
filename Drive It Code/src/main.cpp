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

void loop() {

// ROTARY ENCODER 
{
  // check for encoder button press
  encoder_button.loop();

  // read current encoder state
  curr_CLK_state = digitalRead(encoder_CLK);

  // check for CLK state-change
  // check rising edge ONLY
  if((curr_CLK_state != prev_CLK_state) && curr_CLK_state) {
    // ternary operator
    encoder_direction = digitalRead(encoder_DT) ? cclockwise : clockwise;
    encoder_direction ? --CLK_count : ++CLK_count;
  }
    
  // print movements
  if(curr_CLK_state != prev_CLK_state) {
    String tmpPrint = !encoder_direction ? "Clockwise" : "Counter-Clockwise";
    Serial.println(tmpPrint + "  | Count: " + CLK_count);
  }

  // move current state into previous
  prev_CLK_state = curr_CLK_state;
 
  // check for encoder button press
  if(encoder_button.isPressed())
    Serial.println("The button has been pressed.");
}

// DFplayerMini MP3 PLAYER
{
  // TESTING START

  if((CLK_count > 10) || (CLK_count < -10))
    mp3.playMp3Folder(2);

  // TESTING END

}
}

