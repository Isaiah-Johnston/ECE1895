#include "global.h"


bool     clockwise  = 0;
bool     cclockwise = 1;
uint32_t baud_rate  = 9600;

uint8_t  task_completion_time;
int32_t  curr_time;
int32_t  prev_time;
uint32_t game_run_time; 
uint8_t  task_completion_count_prev;
uint8_t  task_completion_count;
uint8_t  task_completion_goal;
uint8_t  prev_task;
uint8_t  curr_task;
bool     task_success_flag;

enum TASKS {TURN_LEFT  = 0, 
            TURN_RIGHT = 1, 
            ACCELERATE = 2, 
            BRAKE      = 3,
            SHIFT_UP   = 4, 
            SHIFT_DOWN = 5,
            PRESS_HORN = 6, 
            NONE       = 10};


uint8_t  pin_serial_TX = 2;
uint8_t  pin_serial_RX = 3; 

uint8_t  power_SW      = 4;

uint8_t  encoder_CLK   = 5; 
uint8_t  encoder_DT    = 6;  
uint8_t  encoder_SW    = 7;

uint8_t  ignition_SW   = 8;

uint8_t  pin_spi_SS    = 10;
uint8_t  pin_spi_MOSI  = 11;
uint8_t  pin_SPI_MISO  = 12;
uint8_t  pin_SPI_SCK   = 13;

uint8_t  pin_I2C_SDA   = 18;
uint8_t  pin_I2C_SCL   = 19; 

int32_t  encoder_direction;       
int32_t  encoder_CLK_position;
bool     encoder_curr_state;
bool     encoder_prev_state;

uint8_t  Game_Power_On = 1;   
uint8_t  Game_Start    = 2;  
uint8_t  Game_End      = 3;
uint8_t  Turn_Left     = 4;
uint8_t  Turn_Right    = 5;
uint8_t  Accelerate    = 6;
uint8_t  Brake         = 7;
uint8_t  Up_Shift      = 8;     
uint8_t  Down_Shift    = 9;     
uint8_t  Car_Horn      = 10;
uint8_t  Game_Won      = 11;
uint8_t  Game_Lost     = 12;

DFRobotDFPlayerMini mp3;
ezButton            encoder_button(encoder_SW);


////////////////* UTILITIES *////////////////


SoftwareSerial& getSSerial() {
    static SoftwareSerial SSerial(pin_serial_RX, pin_serial_TX);
    return SSerial;
}


bool getPinMode(int pin) {
    uint8_t           bit  = digitalPinToBitMask(pin);
    uint8_t           port = digitalPinToPort(pin);
    volatile uint8_t  *reg = portModeRegister(port);

    if(*reg & bit) 
        return false;

    return true;
}


bool resetPins() {
    for(uint8_t i = 0; i <= 19; i++){
        pinMode(i, OUTPUT);
        if(getPinMode(i)) { return false; }
    }

    return true;
}


void generate_task() {
    
    // generate a random task
    randomSeed(analogRead(0));
/*    curr_task = random(6);

    if(curr_task == prev_task && prev_task != NONE) 
        curr_task = (curr_task < 6) ? curr_task+1 : curr_task-1;
*/
    
    curr_task = PRESS_HORN; // FOR TESTING

    // TODO: optimize for a better "randomness"

    // prompt user with the generated task
    switch(curr_task) {
        case TURN_LEFT:
            Serial.print("current task is 'Turn Left ("); 
            Serial.print(curr_task); delay(1); Serial.print(")..."); delay(1);
            Serial.println(); delay(1);
            // TODO: call mp3 player
            break;
        case TURN_RIGHT:
            Serial.println("current task is 'Turn Right'..."); delay(1); 
            // TODO
            break;
        case ACCELERATE:
            Serial.println("current task is 'Accelerate'..."); delay(1); 
            // TODO
            break;
        case BRAKE:
            Serial.println("current task is 'Brake'..."); delay(1); 
            // TODO
            break;
        case SHIFT_UP:
            Serial.println("current task is 'Upshift'..."); delay(1); 
            // TODO
            break;
        case SHIFT_DOWN:
            Serial.println("current task is 'Downshift'..."); delay(1); 
            // TODO
            break;
        case PRESS_HORN:
            Serial.println("current task is 'Honk'..."); delay(1); 
            // TODO
            break;
        default:
            Serial.println("NO INSTRUCTION GIVEN"); delay(100);
            exit(0);
            break;
    }
    
    // reassign task parameters
    task_success_flag = false;
    prev_task = curr_task;
}


void  task_success() {

    // increment completed task counter
    task_completion_count++;

    // delay time before next task
    prev_time   = curr_time - 750 * task_completion_time;
    //curr_task = NONE;

    // set flag for loop in main.cpp void loop()
    task_success_flag = true;

    // prompt user 
    Serial.print("total tasks completed: "); delay(1);
    Serial.print(task_completion_count);     delay(1);
    Serial.println();                        delay(1);
}


void end_game() {
    
    Serial.println("\nTHE GAME HAS ENDED");          delay(1);
    Serial.println("        ... calculating score"); delay(100);

    // if game ended unsuccesfully
    if(task_completion_count != task_completion_goal) {
        Serial.println("SCORE GOAL NOT MET");         delay(1);
        Serial.print("        ... final score is: "); delay(1); 
        Serial.print(task_completion_count);          delay(1); 
        Serial.println();                             delay(100);
        
        // TODO: speaker output
        // TODO: display output?
    }

    // if game ended succesfully 
    else {
        Serial.println("SCORE GOAL MET");          delay(1);
        Serial.print("      ... user completed "); delay(1); 
        Serial.print(task_completion_count);       delay(1); 
        Serial.print(" tasks");                    delay(1);
        Serial.println();                          delay(100);

        // TODO: speaker output
        // TODO: display output?
    }
    
    // end the program
    exit(0);
}


////////////////* HARDWARE *////////////////


bool check_ignition() { 

    // ignition switch is off
    if(digitalRead(ignition_SW) == HIGH) { return false; }
    
    // ignition switch is on
    return true;
}


bool check_horn() {

    bool horn_state = false;

    // start loop
    encoder_button.loop();

    // horn is activated
    if(encoder_button.isPressed()) { 
        Serial.println("The button has been pressed.");
        horn_state = true;
    }

    /* GAME FUNCTIONALITY */

    // when another task is requested
    if(curr_task != PRESS_HORN && horn_state == true) { end_game();     }
    
    // when horn and corresponding task are active
    if(curr_task == PRESS_HORN && horn_state == true) { task_success(); }
  

    // horn is not active
    return horn_state;
}


bool check_gas() { return true; }           // TODO
bool check_brake() { return true;}          // TODO
uint8_t check_turnSignal() { return 0; }    // TODO


void check_wheel() {

    // read current encoder state
    encoder_curr_state = digitalRead(encoder_CLK);

    // update the encoders previous condition
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


    /* TODO: implement game functionality */
}



