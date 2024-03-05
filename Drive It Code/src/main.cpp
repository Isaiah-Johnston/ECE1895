#include <Arduino.h>

#include "global.h"
#include "initialize.h"



void setup() {

  // if initialization was succesful
  if(initialize()) { return; }

  // if initialization failed, stop program
  Serial.println("INITIALIZATION FAILED..."); delay(1);
  exit(0);

}


void loop() {
  
  // check for conditions to end the game
  if(task_completion_count == task_completion_goal) { end_game(); }
  if(!check_ignition())                             { end_game(); }

  // if 5 seconds have passed
  curr_time = millis();
  if(curr_time - prev_time >= task_completion_time * 1000) {
    
    // if the user did not input anything
    if(!task_success_flag) { end_game(); }

    // reset "timer"
    prev_time = curr_time;

    // generate a new task
    generate_task();
    
  }

  // check hardware for correct/incorrect changes
  check_wheel();
  check_horn();

  // TODO: add a update_display() function and call for refreshing the display
}
