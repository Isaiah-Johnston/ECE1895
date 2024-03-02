#include <Arduino.h>
#include "initialize.h"

void setup() {
  
  // if initialization was successful, start program
  if(initialize()) { return; }

  // if initialization was unsuccessful, end program early and
  // output error code in built-in led
  Serial.println("INITIALIZATION FAILED");
  while(true) {
    pinMode(debug_LED, OUTPUT);
    digitalWrite(debug_LED, HIGH); delay(100);
    digitalWrite(debug_LED, LOW ); delay(100);
  }
}


void loop() {

  encoder_button .loop();
  ignition_switch.loop();

  // check for ignition switch deactivation
  if(ignition_switch.isReleased()) 
    Serial.println("The button has been pressed.");
    // TODO: add functionality

  // check for encoder button press
  if(encoder_button.isPressed())
     Serial.println("The button has been pressed.");
     // TODO: add functionality

  
  // read current encoder state
  encoder_curr_state = digitalRead(encoder_CLK);

  // check for CLK state-change (rising edge ONLY)
  if(encoder_curr_state != encoder_prev_state && encoder_curr_state) {

    // set proper direction of rotation
    encoder_direction = digitalRead(encoder_DT) ? clockwise : cclockwise;
    
    // increment/decrement angular
    encoder_direction ? ++encoder_CLK_position : --encoder_CLK_position;
  }

  // print movements for debugging.
  if(encoder_curr_state != encoder_prev_state) {
    String tmpPrint = !encoder_direction ? "Clockwise" : "Counter-Clockwise";
    Serial.println(tmpPrint + "  | Count: " + encoder_CLK_position);
  }

}

