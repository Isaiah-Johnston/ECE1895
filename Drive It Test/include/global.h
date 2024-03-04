#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

#ifndef GLOBAL_H
#define GLOBAL_H


// static pins
extern uint8_t serial_TX;
extern uint8_t serial_RX; 

extern uint8_t power_SW;  
extern uint8_t ignition_SW;  

extern uint8_t encoder_CLK; 
extern uint8_t encoder_DT;  
extern uint8_t encoder_SW; 

extern uint8_t pin_spi_SS;
extern uint8_t pin_spi_MOSI;
extern uint8_t pin_SPI_MISO;
extern uint8_t pin_SPI_SCK;

// ATmega328p Baud Rate
extern uint32_t baud_rate;

// static vars
extern bool clockwise;
extern bool cclockwise;

// global vars
extern uint8_t task_completion_count;

extern int32_t encoder_direction;       // TODO: OPTIMIZE     
extern int32_t encoder_CLK_position;
extern bool    encoder_curr_state;
extern bool    encoder_prev_state;

// sound effects
extern uint8_t Game_Power_On;       // TODO: MAKE/DOWNLOAD SOUNDS       
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

// objects
extern DFRobotDFPlayerMini mp3;
extern ezButton            encoder_button;


// return software-serial object for use in other .cpp files
extern SoftwareSerial& getSSerial();

// return true for INPUT and false for OUTPUT
extern bool getPinMode(int);

// reset all pins to default state of OUTPUT
extern bool resetPins();


#endif // GLOBAL_H

