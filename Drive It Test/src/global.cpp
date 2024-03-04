#include "global.h"


uint8_t serial_TX    = 2;
uint8_t serial_RX    = 3; 
 
uint8_t power_SW     = 4;   
uint8_t ignition_SW  = 8;

uint8_t encoder_CLK  = 5; 
uint8_t encoder_DT   = 6;  
uint8_t encoder_SW   = 7;

uint8_t pin_spi_SS   = 10;
uint8_t pin_spi_MOSI = 11;
uint8_t pin_SPI_MISO = 12;
uint8_t pin_SPI_SCK  = 13;

uint32_t baud_rate  = 9600;

bool     clockwise  = 0;
bool     cclockwise = 1;

uint8_t task_completion_count;

int32_t encoder_direction;       
int32_t encoder_CLK_position;
bool    encoder_curr_state;
bool    encoder_prev_state;

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

DFRobotDFPlayerMini mp3;
ezButton            encoder_button(encoder_SW);


////////////////////////////////////////////////////////////


SoftwareSerial& getSSerial() {
    static SoftwareSerial SSerial(serial_RX, serial_TX);
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

