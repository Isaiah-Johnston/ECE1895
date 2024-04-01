#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>
#include <U8g2lib.h>

// Global variables
int task_completion_count = 0;
const int task_completion_goal = 50;
const int task_completion_time = 5000;
unsigned long prev_time = 0;
bool task_success_flag = true;
unsigned long curr_time = 10000;
int curr_task;
int required_steps = 10; // Example steps required for turn tasks
int prev_task_position = 0; // To track encoder position at task start

// function prototypes
bool check_ignition();
bool initialize();

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
bool clockwise = false; // Assuming initial direction
bool encoder_direction = false; // Assuming initial direction
int32_t encoder_CLK_position = 0;

// Display
U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, display_clock, display_data, display_cs, display_dc, display_reset);

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/2, /*tx =*/3);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup() {
#if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/pin_serial_RX, /*tx =*/pin_serial_TX);
#else
  FPSerial.begin(9600);
#endif

  Serial.begin(115200); delay(10);

  Serial.println(F("DFRobot DFPlayer Mini Demo")); delay(10);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)")); delay(10);

  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {
    Serial.println(F("Unable to begin:"));
    while(true) {
      delay(0); // Keep compatibility with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online.")); delay(10);
  myDFPlayer.volume(30);  // Set volume value. From 0 to 30

  // Initialize system
  if(!initialize()) {
    Serial.println("INITIALIZATION FAILED... Stopping program."); delay(100);
    exit(0);
  }
}

bool initialize() {
  Serial.begin(9600);
  Serial.println("Serial ports initialized."); delay(1);

  honk_button.setDebounceTime(50);
  brake_button.setDebounceTime(50);
  accel_button.setDebounceTime(50);
  Serial.println("Buttons initialized."); delay(1);

  u8g2.begin();
  Serial.println("Display initialized."); delay(100);

  pinMode(encoder_CLK, INPUT);
  pinMode(encoder_DT, INPUT);
  pinMode(ignition_SW, INPUT);

  return true;
}

bool check_ignition() {
  // Example: Assuming LOW means ignition is ON
  return digitalRead(ignition_SW) == LOW;
}

void generate_task() {
  randomSeed(analogRead(0));
  curr_task = random(1, 6); // Generate a random number between 1 and 5
  Serial.print("Generated task: ");
  Serial.println(curr_task);
  switch (curr_task) {
    case 1:
      Serial.println("Current task is 'Honk'");
      break;
    case 2:
      Serial.println("Current task is 'Turn Left'");
      prev_task_position = encoder_CLK_position;
      break;
    case 3:
      Serial.println("Current task is 'Turn Right'");
      prev_task_position = encoder_CLK_position;
      break;
    case 4:
      Serial.println("Current task is 'Brake'");
      break;
    case 5:
      Serial.println("Current task is 'Accelerate'");
      break;
    default:
      Serial.println("Invalid task generated");
      break;
  }
  myDFPlayer.playMp3Folder(curr_task); // Play sound associated with the task
}

void task_success() {
  task_completion_count++;
  Serial.println("Task completed successfully.");
  curr_time = millis();
  prev_time = curr_time - 0.75 * task_completion_time; // Reset task timer
  task_success_flag = true;
  Serial.print("Total tasks completed: ");
  Serial.println(task_completion_count);
}

void end_game() {
  Serial.println("\nTHE GAME HAS ENDED");
  delay(100);
  if (task_completion_count != task_completion_goal) {
    Serial.println("SCORE GOAL NOT MET");
    myDFPlayer.play(6); // Example failure sound
  } else {
    Serial.println("SCORE GOAL MET");
    // Example success sound
  }
  // Show final score or win/loss message
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    if (task_completion_count != task_completion_goal) {
      u8g2.drawStr(0, 24, "Score:");
      u8g2.drawStr(60, 24, String(task_completion_count).c_str());
    } else {
      u8g2.drawStr(0, 24, "WINNER!");
    }
  } while (u8g2.nextPage());
  delay(3000); // Display message for 3 seconds before exiting
  exit(0);
}

void check_horn() {
  if (curr_task == 1 && honk_button.isPressed()) {
    task_success();
  } else if (curr_task != 1 && honk_button.isPressed()) {
    end_game();
  }
}

void check_brake() {
  if (curr_task == 4 && brake_button.isPressed()) {
    task_success();
  } else if (curr_task != 4 && brake_button.isPressed()) {
    end_game();
  }
}

void check_accel() {
  if (curr_task == 5 && accel_button.isPressed()) {
    task_success();
  } else if (curr_task != 5 && accel_button.isPressed()) {
    end_game();
  }
}

void check_wheel() {
  encoder_curr_state = digitalRead(encoder_CLK);
  if ((encoder_curr_state != encoder_prev_state) && encoder_curr_state) {
    encoder_direction = digitalRead(encoder_DT) ? clockwise : !clockwise;
    encoder_direction ? ++encoder_CLK_position : --encoder_CLK_position;
  }
  encoder_prev_state = encoder_curr_state;
  // Implement logic for turn tasks based on encoder movement
  if ((curr_task == 2 || curr_task == 3) && abs(encoder_CLK_position - prev_task_position) >= required_steps) {
    task_success();
  }
}

void update_display() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 24, "Task:");
    u8g2.drawStr(60, 24, String(curr_task).c_str());
    u8g2.drawStr(0, 48, "Score:");
    u8g2.drawStr(60, 48, String(task_completion_count).c_str());
  } while (u8g2.nextPage());
}

void loop() {
  if (!check_ignition()) {
    Serial.println("Ignition Off. Ending Game.");
    end_game();
  }
  curr_time = millis();
  if (curr_time - prev_time >= task_completion_time) {
    if (!task_success_flag) {
      end_game();
    }
    prev_time = curr_time;
    generate_task();
    task_success_flag = false; // Reset flag for next task
  }
  check_wheel();
  check_horn();
  check_brake();
  check_accel();
  update_display(); // Refresh display with current game state
}
