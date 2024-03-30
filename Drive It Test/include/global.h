#include <Arduino.h>
#include <String.h>
#include <SoftwareSerial.h>

#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>

#ifndef GLOBAL_H
#define GLOBAL_H

// GLOBAL VARS
extern int32_t baud_rate;
extern int8_t  task_goal,
               task_time,
               encoder_tol;

// PIN VARS
extern uint8_t IGNITION_SW,
               HORN_BTN,
               ACCELERATION_SW,
               BRAKE_SW,
               ENCODER_CLK,
               ENCODER_DT,
               SERIAL_RX,
               SERIAL_TX;

// TIME-KEEPING VARS
extern int32_t curr_time,
               prev_time;

// TASK VARS
extern int8_t  curr_task,
               prev_task,
               task_count,             
               PRESS_HORN,
               PRESS_GAS,
               PRESS_BRAKE,
               TURN_LEFT,
               TURN_RIGHT,
               TURN_LEFT_HARD,
               TURN_RIGHT_HARD;
extern bool    task_flag;

// ENCODER STATE-KEEPING VARS
extern int8_t curr_position,
              prev_position;
extern bool   curr_state,
              prev_state;

// CLASS OBJECTS
extern DFRobotDFPlayerMini mp3;
extern ezButton            horn;
extern ezButton            gas;
extern ezButton            brake;

// PRIMARY FUNCTIONS
extern bool initialize();
extern void generate_task();
extern void task_success();
extern void end_game();

// HARDWARE CHECK FUNCTIONS
extern bool check_ignition();
extern void check_horn();
extern void check_gas();
extern void check_brake();
extern void check_wheel();

// OTHER/MISC FUNCTION
extern SoftwareSerial getSSerial();

#endif