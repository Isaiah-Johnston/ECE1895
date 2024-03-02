#include <initialize.h>

uint8_t serial_TX   = 2;
uint8_t serial_RX   = 3;  
uint8_t power_SW    = 4;    
uint8_t encoder_CLK = 5; 
uint8_t encoder_DT  = 6;  
uint8_t encoder_SW  = 7;
uint8_t ignition_SW = 8; 
uint8_t debug_LED   = 13;

uint32_t baud_rate  = 9600;
bool     clockwise  = 1;
bool     cclockwise = 0;

uint8_t Game_Power_On = 1;   
uint8_t Game_Start    = 2;  
uint8_t Turn_On_Car   = 3;
uint8_t Turn_Off_Car  = 4;
uint8_t Turn_Left     = 5;
uint8_t Turn_Right    = 6;
uint8_t Accelerate    = 7;
uint8_t Brake         = 8;
uint8_t Up_Shift      = 9;
uint8_t Down_Shift    = 10;
uint8_t Car_Horn      = 11;
uint8_t Game_Won      = 12;
uint8_t Game_Lost     = 13;

int32_t encoder_direction;       
int32_t encoder_CLK_position;
bool    encoder_curr_state;
bool    encoder_prev_state;

DFRobotDFPlayerMini mp3;
ezButton            encoder_button(encoder_SW);
ezButton            ignition_switch(ignition_SW);

// Performs all actions a typical setup() would
bool initialize() {

    // begin serial communications
    Serial.begin(baud_rate);
    getSSerial().begin(baud_rate);

    //////////// I/O ///////////// 

    // reset all pins to default state
    if(!resetPins()) { return false; }

    // set I/O types and check for errors
    pinMode(debug_LED,   OUTPUT); if( getPinMode(debug_LED)  ) { return false; }
    pinMode(power_SW,    INPUT ); if(!getPinMode(power_SW)   ) { return false; }
    pinMode(encoder_CLK, INPUT ); if(!getPinMode(encoder_CLK)) { return false; }
    pinMode(encoder_DT,  INPUT ); if(!getPinMode(encoder_DT) ) { return false; }

    Serial.println("I/O INITIALIZED...");

    //////////// ROTARY ENCODER ///////////// 
    
    // set button debouncing period
    encoder_button.setDebounceTime(100);

    // initialize previous state for encoder position
    encoder_prev_state = digitalRead(encoder_CLK);

    Serial.println("ENCODER INITIALIZED...");

    //////////// DFPlayer MP3 /////////////

    // set volume 0-30
    mp3.volume(20);

    // check for proper connection
    if(!mp3.begin(getSSerial())) { return false; }
    
    // play startup sound effect
    mp3.playMp3Folder(Game_Power_On);

    Serial.println("DFPLAYER INITIALIZED...");

    //////////// IGNITION SWITCH /////////////

    // set switch debouncing period
    ignition_switch.setDebounceTime(100);

    // wait for user to activate switch
    ignition_switch.loop();
    while(!ignition_switch.isPressed()) { delay(1); }

    //////////// END INITIALIZATION /////////////

    return true;
}    



