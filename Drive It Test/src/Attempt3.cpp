#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>
#include <U8g2lib.h>
#include <SoftwareSerial.h>

// Global variables
int task_completion_count = 0;
const int task_completion_goal = 50;
const unsigned long task_completion_time = 5000;
unsigned long prev_time = 0;
bool task_success_flag = true;
unsigned long curr_time = 10000;
int curr_task;

// Pins
const uint8_t encoder_CLK = 5;
const uint8_t encoder_DT = 6;
const uint8_t ignition_SW = A1;
const uint8_t pin_serial_TX = 3;
const uint8_t pin_serial_RX = 2;
const uint8_t power_SW = 4;
const uint8_t display_clock = 13;
const uint8_t display_data = 11;
const uint8_t display_cs = 10;
const uint8_t display_dc = 9;
const uint8_t display_reset = 8;

// Buttons
ezButton honk_button(A5);
ezButton brake_button(A4);
ezButton accel_button(A3);

// Wheel encoder variables
bool encoder_prev_state = false;
bool encoder_curr_state = false;
bool clockwise = true; // Assuming initial direction
bool encoder_direction = false; // Assuming initial direction
int32_t encoder_CLK_position = 0;

// Display
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, display_clock, display_data, display_cs, display_dc, display_reset);

SoftwareSerial softSerial(2, 3); // RX, TX
#define FPSerial softSerial

DFRobotDFPlayerMini myDFPlayer;

void setup() {
  FPSerial.begin(9600);
  Serial.begin(115200);
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, true, true)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true); // Halt execution on failure
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(30);

  if(!initialize()) {
    Serial.println("Initialization failed...");
    while(true); // Halt execution on failure
  }
}

bool initialize() {
  Serial.println("Serial ports initialized.");

  // Initialize buttons
  honk_button.setDebounceTime(50);
  brake_button.setDebounceTime(50);
  accel_button.setDebounceTime(50);
  Serial.println("Buttons initialized.");

  // Initialize display
  u8g2.begin();
  Serial.println("Display initialized.");

  return true;
}

void generate_task() {
  randomSeed(analogRead(0));
  curr_task = random(1, 6); // Generate a random number between 1 and 5
  Serial.print("Generated task: ");
  Serial.println(curr_task);

  switch (curr_task) {
    case 2:
      Serial.println("Current task is 'Turn Left'");
      break;
    case 3:
      Serial.println("Current task is 'Turn Right'");
      break;
    case 5:
      Serial.println("Current task is 'Accelerate'");
      break;
    case 4:
      Serial.println("Current task is 'Brake'");
      break;
    case 1:
      Serial.println("Current task is 'Honk'");
      break;
    default:
      Serial.println("Invalid task generated");
      break;
  }
  myDFPlayer.playMp3Folder(curr_task);
}

void task_success() {
  task_completion_count++;
  Serial.println("Task completed successfully.");
  curr_time = millis();
  prev_time = curr_time - (task_completion_time * 3 / 4);
  task_success_flag = true;

  Serial.print("Total tasks completed: ");
  Serial.println(task_completion_count);
}

void end_game() {
  char scoreStr[20];
  Serial.println("\nTHE GAME HAS ENDED");
  Serial.println("        ... calculating score");

  if (task_completion_count != task_completion_goal) {
    sprintf(scoreStr, "Score: %d", task_completion_count);
    Serial.println("SCORE GOAL NOT MET");
    Serial.println(scoreStr);
    myDFPlayer.play(6);
  } else {
    Serial.println("SCORE GOAL MET");
    Serial.print("      ... user completed ");
    Serial.print(task_completion_count);
    Serial.println(" tasks");
  }

  while(true); // Halt execution
}

void check_wheel() {
  encoder_curr_state = digitalRead(encoder_CLK);
  if ((encoder_curr_state != encoder_prev_state) && encoder_curr_state) {
    encoder_direction = digitalRead(encoder_DT) != clockwise;
    if (encoder_direction) {
      encoder_CLK_position++;
    } else {
      encoder_CLK_position--;
    }
  }
  encoder_prev_state = encoder_curr_state;
}

void check_horn() {
  honk_button.loop(); // Must call the loop() function first
  bool horn_status = honk_button.isPressed();
  if (horn_status) {
    if (curr_task == 1) {
      task_success();
    } else {
      end_game();
    }
  }
}

void check_brake() {
  brake_button.loop();
  bool brake_status = brake_button.isPressed();
  if (brake_status) {
    if (curr_task == 4) {
      task_success();
    } else {
      end_game();
    }
  }
}

void check_accel() {
  accel_button.loop();
  bool accel_status = accel_button.isPressed();
  if (accel_status) {
    if (curr_task == 5) {
      task_success();
    } else {
      end_game();
    }
  }
}

void loop() {
  if (task_completion_count == task_completion_goal) { end_game(); }

  curr_time = millis();
  if (curr_time - prev_time >= task_completion_time) {
    if (!task_success_flag) { end_game(); }
    prev_time = curr_time;
    generate_task();
    task_success_flag = false; // Reset success flag for new task
  }

  check_wheel();
  check_horn();
  check_brake();
  check_accel();
}

