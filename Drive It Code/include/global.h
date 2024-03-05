#include <Arduino.h>
#include <String.h>

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

#ifndef GLOBAL_H
#define GLOBAL_H


// global stds
extern bool     clockwise;                  // denotes direction of spin for encoder
extern bool     cclockwise;                 // 
extern uint32_t baud_rate;                  // sampling rate of ATmega328p, for serial comms.

extern int32_t  curr_time;                  // stores time values for use in timing events in 
extern int32_t  prev_time;                  // main loop
extern uint32_t game_run_time;              //
extern uint8_t  task_completion_time;       //
extern uint8_t  task_completion_count_prev; // stores completed task values
extern uint8_t  task_completion_count;      //
extern uint8_t  task_completion_goal;       //
extern uint8_t  prev_task;                  // stores task types for users to complete
extern uint8_t  curr_task;                  //
extern bool     task_success_flag;          // indicates if user provided proper input within
                                            // given time period
// static pins
extern uint8_t pin_serial_TX;               // UART output pins
extern uint8_t pin_serial_RX;               //

extern uint8_t power_SW;                    // "single action" input pins
extern uint8_t ignition_SW;                 // 

extern uint8_t encoder_CLK;                 // rotary encoder input pins
extern uint8_t encoder_DT;                  // 
extern uint8_t encoder_SW;                  //

extern uint8_t pin_spi_SS;                  // SPI output pins
extern uint8_t pin_spi_MOSI;                //
extern uint8_t pin_SPI_MISO;                //
extern uint8_t pin_SPI_SCK;                 // 

extern uint8_t pin_I2C_SCL;                 // I2C output pins 
extern uint8_t pin_I2C_SDA;                 //

// vars
extern int32_t encoder_direction;           // rotary encoder data variables   
extern int32_t encoder_CLK_position;        //
extern bool    encoder_curr_state;          // TODO: OPTIMIZE/MINIMIZE
extern bool    encoder_prev_state;          // 

// sound effects
extern uint8_t Game_Power_On;               // mp3 file namespace   
extern uint8_t Game_Start;                  //
/*                                          // TODO: FURTHER IMPLEMENT
extern uint8_t Turn_On_Car;                 // TODO: OPTIMIZE?
extern uint8_t Turn_Off_Car;                //   
extern uint8_t Turn_Left;                   //
extern uint8_t Turn_Right;                  //
extern uint8_t Accelerate;                  //
extern uint8_t Brake;                       //
extern uint8_t Up_Shift;                    //
extern uint8_t Down_Shift;                  //  
extern uint8_t Car_Horn;                    //
extern uint8_t Game_Won;                    //
extern uint8_t Game_Lost;                   //
*/

// objects
extern DFRobotDFPlayerMini mp3;             // used for calling DFplayer mp3 library functions
extern ezButton            encoder_button;  // used for debouncing/other functionalities of encoder button


/* GLOBAL UTILITY FUNCTIONS */


extern SoftwareSerial& getSSerial();    // return software-serial object for use in other .cpp files
extern bool            getPinMode(int); // return true for INPUT and false for OUTPUT
extern bool            resetPins();     // reset all pins to default state of OUTPUT
extern void            generate_task(); // generate a task for the user to complete
extern void            task_success();  // increments completed task counted and initiates next task
extern void            end_game();      // end the game if called


/* GLOBAL HARDWARE CHECK FUNCTIONS */


extern bool    check_ignition();        // NO switch with INPUT_PULLUP pin designation
extern bool    check_horn();            // built-in rotary encoder button
extern bool    check_gas();             // NO limit switch with INPUT_PULLUP pin designation
extern bool    check_brake();           // NO limit switch with INPUT_PULLUP pin designation
extern uint8_t check_turnSignal();      // NO-NC switch 
extern void    check_wheel();           // rotary encoder

#endif // GLOBAL_H

