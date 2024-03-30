#include "global.h"

// GLOBAL VARS
int32_t baud_rate        = 9600;
int8_t  task_goal        = 10,
        task_time        = 5,       // Time to complete tasks.   
        encoder_tol      = 3;       // Allowed ppr before considering "action"

// PIN VARS
uint8_t IGNITION_SW     = 8,
        HORN_BTN        = 7,
        ACCELERATION_SW = 13,
        BRAKE_SW        = 12,
        ENCODER_CLK     = 5,
        ENCODER_DT      = 6,
        SERIAL_RX       = 3,
        SERIAL_TX       = 2;

// TIME-KEEPING VARS
int32_t curr_time,      // For task generation loop
        prev_time;

// TASK VARS
int8_t  curr_task,              // Current task for user to complete
        prev_task,               
        task_count      = 0,    // Total number of completed tasks
        PRESS_HORN      = 0,    // Namespace for tasks
        PRESS_GAS       = 1,
        PRESS_BRAKE     = 2,
        TURN_LEFT       = 3,
        TURN_RIGHT      = 4,
        TURN_LEFT_HARD  = 5,
        TURN_RIGHT_HARD = 6;
bool    task_flag       = true; // For task generation loop, stores
                                // if the current task has been completed

// ENCODER STATE-KEEPING VARS
int8_t curr_position,
       prev_position;       // TRY SETTING TO 0 IF ERRORS OCCUR
bool   curr_state,
       prev_state;

// CLASS OBJECTS
DFRobotDFPlayerMini mp3;
ezButton            horn(HORN_BTN);         // For debouncing
ezButton            gas(ACCELERATION_SW);
ezButton            brake(BRAKE_SW);



bool initialize() {

{ /* SERIAL PORTS*/

    Serial.begin(baud_rate);
    getSSerial().begin(baud_rate);    
    
    Serial.println("INITIALIZATION STARTED");
        delay(1);

    Serial.println("        ... beginning serial communications"); 
        delay(1);
}
    
{ /* PIN MODES */
    
    Serial.println("        ... setting pin modes"); 
        delay(1);

    // Set pin modes for I/O
    pinMode(IGNITION_SW,     INPUT_PULLUP);
    pinMode(HORN_BTN,        INPUT_PULLUP);
    pinMode(ACCELERATION_SW, INPUT_PULLUP);
    pinMode(BRAKE_SW,        INPUT_PULLUP);
    pinMode(ENCODER_CLK,     INPUT       );
    pinMode(ENCODER_DT,      INPUT       );
}
    
    // Initialize encoders previous state
    prev_state = digitalRead(ENCODER_CLK);


{ /* GAME OUTPUTS */

    Serial.println("        ... initializing game outputs"); 
        delay(1);

    // MP3 PLAYER
    // DISPLAY
}

    Serial.println("INITIALIZATION COMPLETE");
        delay(1);

// If everything was successful
return true;
}

void generate_task() {

    randomSeed(millis());

    // Avoid repeating tasks
    while(curr_task == prev_task)
        curr_task = random(3);


    Serial.print("current task is: ");
    Serial.print(curr_task);
    Serial.print(" ...\n");
        delay(1);

    // Set global vars accordingly
    prev_task = curr_task;
    task_flag = false;
    prev_time = curr_time;
}

void task_success() {

    // Set global vars accordingly
    task_count++;
    task_flag = true;
    
    // Create short delay before next task
    prev_time = curr_time - 750*task_time;

    Serial.print("total tasks completed: ");
    Serial.print(task_count);
    Serial.println();
    Serial.println();
        delay(1);

    // TODO: speaker/display outputs

    // Allow settling time for wheel/inputs
    delay(250);
}

void end_game() {
    
    Serial.println("THE GAME HAS ENDED");
        delay(1);

    // If the user WON
    if(task_count == task_goal) {
        Serial.println("SCORE GOAL MET");          
        Serial.print("      ... user completed "); 
        Serial.print(task_count);                   
        Serial.print(" tasks");                    
        Serial.println();                          
            delay(100);
        
        // TODO: mp3/display output

        exit(0);
    }
    // If the user LOST
    else {
        Serial.println("SCORE GOAL NOT MET");
        Serial.print("      ... user completed ");
        Serial.print(task_count);
        Serial.print(" tasks");
        Serial.println();
            delay(100);

        // TODO: mp3/display output

        exit(0);
    }
}

bool check_ignition() {

    // If car is OFF
    if(digitalRead(IGNITION_SW) == HIGH) 
        return false;

    // If car is ON
    return true;
}

void check_horn() {

    // Start debouncing 
    horn.loop();

    if(horn.isPressed()) {
        Serial.println("horn pressed...");
            delay(1);
    }

    // If CORRECT action 
    if(curr_task == PRESS_HORN && horn.isPressed())
        task_success();

    // If INCORRECT action 
    if(curr_task != PRESS_HORN && horn.isPressed())
        end_game();
}

void check_gas() {

    // Start debouncing
    gas.loop();

    if(gas.isPressed()) {
        Serial.println("gas pressed...");
            delay(1);
    }

    // If CORRECT action
    if(curr_task == PRESS_GAS && gas.isPressed())
        task_success();

    // If INCORRECT action
    if(curr_task != PRESS_GAS && gas.isPressed())
        end_game();
}

void check_brake() {
    
    // Start debouncing
    brake.loop();

    if(brake.isPressed()) {
        Serial.println("brake pressed...");
            delay(1);
    }

    // If CORRECT action
    if(curr_task == PRESS_BRAKE && brake.isPressed())
        task_success();

    // If INCORRECT action
    if(curr_task != PRESS_BRAKE && brake.isPressed())
        end_game();
}

void check_wheel() {

    // Read current encoder state
    curr_state = digitalRead(ENCODER_CLK);

    // Increment the encoder's position accordingly
    if(curr_state != prev_state && curr_state) {
        if(curr_state != digitalRead(ENCODER_DT)) 
            --curr_position;
        else 
            ++curr_position;
    }

    if(curr_state != prev_state && curr_position != prev_position) {
        Serial.print("Wheel Position: ");
        Serial.print(curr_position);
        Serial.println();
    }

    // Update the previous state vars
    prev_position = curr_position;
    prev_state    = curr_state;

    // If wheel movement greater than allowed tolerance occurs
    if(curr_position >= encoder_tol) {
        
        // Check if CORRECT/INCORRECT action
        if(curr_task == TURN_LEFT) 
            task_success();
        else 
            end_game(); 
    }

    // If wheel movement greater than allowed tolerance occurs
    if(curr_position <= -1*encoder_tol) {

        // Check if CORRECT/INCORRECT action
        if(curr_task == TURN_RIGHT) 
            task_success();
        else  
            end_game(); 
    }
}

SoftwareSerial getSSerial() {
    static SoftwareSerial SSerial(SERIAL_RX, SERIAL_TX);
    return SSerial;
}