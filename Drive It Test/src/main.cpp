#include "global.h"

void setup() {

  // Runs initialization sequence and checks for failure
  if(!initialize()) {
    Serial.println("INITIALIZATION FAILED..."); delay(100);
    exit(0);
  }

  Serial.println("WAITING FOR PLAYER");
    delay(1);

  // Wait for user to start game
  while(!check_ignition())
    delay(1);

  Serial.println("THE GAME HAS STARTED");
    delay(1);
}


void loop() { 

{ /* HANDLES GAME-END CONDITIONS */

  // If user turns vehicle off
  if(!check_ignition())
    end_game();

  // If user reaches task goal
  if(task_count == task_goal) 
    end_game();
}


{ /* HANDLES TASK GENERATION */

  curr_time = millis();

  // Generates a new task every task_time seconds
  // Checks for user inaction
  if(curr_time-prev_time >= task_time*1000) {
    
    // If user didn't do anything
    if(!task_flag)
      end_game();

    // Generate task
    generate_task();
  }
}

{ /* CHECKS IO FOR USER ACTION */
  check_horn();
  check_gas();
  check_brake();
  check_wheel();
}
};