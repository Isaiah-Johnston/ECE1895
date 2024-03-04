#include <Arduino.h>
#include <String.h>

#include "global.h"
#include "initialize.h"

bool check_ignition(); 
void check_encoder();

void setup() {
  
{ /* RUN INITIALIZATIONS */

  // exit program if initialization failed
  if(!initialize()) {
    Serial.println("INITIALIZATION FAILED..."); delay(1);
    exit(0);
  }

  Serial.println("INITIALIZATION SUCCESSFUL");
}


{ /* WAIT FOR GAME BEGIN */

    // wait for user to activate ignition switch
    Serial.println("        ... waiting for user"); delay(1);
    while(digitalRead(ignition_SW) == LOW) { delay(1); } 
    delay(1);

    // game begins when user activates ignition switch
    Serial.println("!!! GAME BEGIN !!!!");
}

  delay(1);
}


void loop() {

  if(!check_ignition()) {
    Serial.println("THE IGNITION SWITCH IS OFF"); delay(100);
    // TODO: start process of ending the game
    exit(0);
  }

  check_encoder();
  
}


bool check_ignition() { 

  if(!digitalRead(ignition_SW))
    return false;
  
  return true;
}



void check_encoder() {

  // check for encoder button press
  encoder_button.loop();

  // read current encoder state
  encoder_curr_state = digitalRead(encoder_CLK);

  int32_t prev_encoder_CLK_position = encoder_CLK_position;

  // check for CLK state-change
  // check rising edge ONLY
  if((encoder_curr_state != encoder_prev_state) && encoder_curr_state) {
    // ternary operator
    encoder_direction = digitalRead(encoder_DT) ? clockwise : cclockwise;
    encoder_direction ? ++encoder_CLK_position : --encoder_CLK_position;
  }
    
  // print movements
  if(encoder_curr_state != encoder_prev_state && encoder_CLK_position != prev_encoder_CLK_position) {
    String tmpPrint = !encoder_direction ? "Counter-Clockwise (LEFT)  " : "Clockwise         (RIGHT) ";
    Serial.println(tmpPrint + "| Count: " + encoder_CLK_position);
  }

  // move current state into previous
  encoder_prev_state = encoder_curr_state;
 
  // check for encoder button press
  if(encoder_button.isPressed())
    Serial.println("The button has been pressed.");

}

