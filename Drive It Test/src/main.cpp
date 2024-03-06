#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// static pins
#define serial_TX   2
#define serial_RX   3


// def objects
DFRobotDFPlayerMini mp3;
SoftwareSerial      softwareSerial(serial_RX, serial_TX);

void setup() {

  Serial.begin(9600);
  softwareSerial.begin(9600);

// DFplayerMini MP3 PLAYER
{
  // start comms with device
  if(mp3.begin(softwareSerial)) {
    Serial.println("Connection to DFplayerMini successful!");

    // set volume 0-30
    mp3.volume(10);

    // play 0001.mp3 in "mp3" folder on SD
    mp3.playMp3Folder(1);
  }
  else 
    Serial.println("Connection to DFplayerMini failed!");
}

}

void loop() { };