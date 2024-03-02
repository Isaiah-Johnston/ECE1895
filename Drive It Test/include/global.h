#ifdef _INITIALIZE_H
    #define extern
#else

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

// static pins
extern uint8_t serial_TX;
extern uint8_t serial_RX;  
extern uint8_t power_SW;    
extern uint8_t encoder_CLK; 
extern uint8_t encoder_DT;  
extern uint8_t encoder_SW; 
extern uint8_t ignition_SW;  
extern uint8_t debug_LED;
 

//////////////////////
// TODO: INTERRUPTS //
//////////////////////

// static vars
extern uint32_t baud_rate;
extern bool     clockwise;
extern bool     cclockwise;

// sound effects
extern uint8_t Game_Power_On;         
extern uint8_t Game_Start;
extern uint8_t Turn_On_Car;
extern uint8_t Turn_Off_Car;
extern uint8_t Turn_Left;
extern uint8_t Turn_Right;
extern uint8_t Accelerate;
extern uint8_t Brake;
extern uint8_t Up_Shift;
extern uint8_t Down_Shift;
extern uint8_t Car_Horn;
extern uint8_t Game_Won;
extern uint8_t Game_Lost;


// global vars
extern int32_t encoder_direction;     
extern int32_t encoder_CLK_position;
extern bool    encoder_curr_state;
extern bool    encoder_prev_state;

extern DFRobotDFPlayerMini mp3;
extern ezButton            encoder_button;
extern ezButton            ignition_switch;


#endif