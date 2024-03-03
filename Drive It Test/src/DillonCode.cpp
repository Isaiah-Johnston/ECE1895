#include <Arduino.h>
// Ensure you include the DFRobotDFPlayerMini library
#include <DFRobotDFPlayerMini.h>
// Ensure you include the ezButton library
#include <ezButton.h>

// Pin definitions
uint8_t serial_TX = 2;
uint8_t serial_RX = 3;  
uint8_t power_SW = 4;    
uint8_t encoder_CLK = 5; 
uint8_t encoder_DT = 6;  
uint8_t encoder_SW = 7;
uint8_t ignition_SW = 8; 
uint8_t debug_LED = 13;
uint8_t BrakePin = 9; // Brake functionality pin

// Additional definitions for setup and runtime behavior
uint32_t baud_rate = 9600;
bool clockwise = true;
bool cclockwise = false;
int32_t encoder_direction;       
int32_t encoder_CLK_position = 0;
bool encoder_curr_state;
bool encoder_prev_state;
int BrakeState = 0; // State of the brake

// Game control definitions
uint8_t Game_Power_On = 1;   
uint8_t Game_Start = 2;  
uint8_t Turn_On_Car = 3;
uint8_t Turn_Off_Car = 4;
uint8_t Turn_Left = 5;
uint8_t Turn_Right = 6;
uint8_t Accelerate = 7;
uint8_t Brake = 8; // Brake signal
uint8_t Up_Shift = 9;
uint8_t Down_Shift = 10;
uint8_t Car_Horn = 11;
uint8_t Game_Won = 12;
uint8_t Game_Lost = 13;

DFRobotDFPlayerMini mp3;
ezButton encoder_button(encoder_SW);
ezButton ignition_switch(ignition_SW);

// Function prototypes
bool initialize();
HardwareSerial& getSSerial();

bool initialize() {
    Serial.begin(baud_rate);
    // Placeholder for actual serial port used for DFPlayerMini
    getSSerial().begin(baud_rate);

    // I/O initialization
    pinMode(debug_LED, OUTPUT); 
    if (digitalRead(debug_LED) == HIGH) { // Simple check; adjust according to your setup
        Serial.println("Debug LED pin mode set correctly.");
    } else {
        Serial.println("Error setting debug LED pin mode.");
        return false;
    }
    
    pinMode(power_SW, INPUT);
    pinMode(encoder_CLK, INPUT);
    pinMode(encoder_DT, INPUT);

    Serial.println("I/O INITIALIZED...");

    encoder_button.setDebounceTime(100);
    encoder_prev_state = digitalRead(encoder_CLK);
    Serial.println("ENCODER INITIALIZED...");

    mp3.volume(20);
    if (!mp3.begin(getSSerial())) {
        Serial.println("DFPLAYER initialization failed.");
        return false;
    }
    mp3.playMp3Folder(Game_Power_On);
    Serial.println("DFPLAYER INITIALIZED...");

    ignition_switch.setDebounceTime(100);

    Serial.println("Waiting for ignition...");
    ignition_switch.loop();
    while (!ignition_switch.isPressed()) { delay(1); }
    Serial.println("Ignition confirmed.");

    return true;
}

void setup() {
    // Attempt to initialize all components
    if (!initialize()) {
        Serial.println("INITIALIZATION FAILED");
        while (true) {
            digitalWrite(debug_LED, HIGH);
            delay(100);
            digitalWrite(debug_LED, LOW);
            delay(100);
        }
    }

    // Additional setup for brake functionality
    pinMode(BrakePin, INPUT);
    pinMode(debug_LED, OUTPUT); // Assuming debug_LED reuses for brake indication
}

void loop() {
    encoder_button.loop();
    ignition_switch.loop();

    if (ignition_switch.isReleased()) {
        Serial.println("Ignition switch released.");
        // Additional functionality as needed
    }

    if (encoder_button.isPressed()) {
        Serial.println("Encoder button pressed.");
        // Additional functionality as needed
    }

    encoder_curr_state = digitalRead(encoder_CLK);
    if (encoder_curr_state != encoder_prev_state && encoder_curr_state) {
        encoder_direction = digitalRead(encoder_DT) ? clockwise : cclockwise;
        encoder_direction ? ++encoder_CLK_position : --encoder_CLK_position;
    }

    if (encoder_curr_state != encoder_prev_state) {
        String directionPrint = encoder_direction ? "Counter-Clockwise" : "Clockwise";
        Serial.println(directionPrint + " | Count: " + encoder_CLK_position);
    }

    // Brake functionality integrated into the loop
    BrakeState = digitalRead(BrakePin);
    if (BrakeState == HIGH) {
        digitalWrite(debug_LED, HIGH);
    } else {
        digitalWrite(debug_LED, LOW);
    }

    encoder_prev_state = encoder_curr_state; // Update previous state for next loop iteration
}

// Placeholder for getSSerial() function. Replace with actual serial port used for DFPlayerMini
HardwareSerial& getSSerial() {
    return Serial; // Adjust this to the correct serial port if needed
}
