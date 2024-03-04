#include "initialize.h"

bool initialize() {

{ /* SERIAL PORTS */
    
    // start serial communications
    Serial.begin(baud_rate);
    getSSerial().begin(baud_rate);

    // finished serial initialization
    Serial.println("INITIALIZATION STARTED...");                  delay(1);
    Serial.println("        ... beginning serial communication"); delay(1);
    Serial.println("SERIAL INITIALIZED");                         delay(1);
}



{ /* IO */
    
    // reset pins and error check
    Serial.println("        ... resetting pins"); delay(1);
    if(!resetPins()) { return false; }
    

    // set I/O types and error check 
    Serial.println("        ... setting pin types"); delay(1);
    pinMode(debug_LED,   OUTPUT); if( getPinMode(debug_LED)  ) { return false; }
    pinMode(power_SW,    INPUT ); if(!getPinMode(power_SW)   ) { return false; }
    pinMode(encoder_CLK, INPUT ); if(!getPinMode(encoder_CLK)) { return false; }
    pinMode(encoder_DT,  INPUT ); if(!getPinMode(encoder_DT) ) { return false; }

    // finished I/O initialization 
    Serial.println("I/O INITIALIZED"); delay(1);
}


{ /* ROTARY ENCODER */

    // set debouncing period for built-in encoder button
    Serial.println("        ... enabling encoder button debouncing"); delay(1);
    encoder_button.setDebounceTime(50);

    // define initial position for encoder
    Serial.println("        ... defining initial state for encoder"); delay(1);
    encoder_prev_state = digitalRead(encoder_CLK);

    // finished rotary encoder initialization
    Serial.println("ROTARY ENCODER INITIALIZED"); delay(1);
}


{ /* MP3 PLAYER */

/*
    // check for proper connection
    Serial.println("        ... establishing connection with mp3 player"); delay(1);
    if(mp3.begin(getSSerial())) { return false; }

    // set volume of speaker 0-30
    Serial.println("        ... setting mp3 player volume"); delay(1);
    mp3.volume(20);

    // TODO: play power-up sound?
    Serial.println("        ... playing startup mp3"); delay(1);
    mp3.playMp3Folder(1);

    // finished mp3 player initialization
    Serial.println("MP3 PLAYER INITIALIZED"); delay(1);
*/
}


{ /* BUTTONS  */

    // TODO

}


{ /* SWITCHES */

    // TODO

}


{ /* SPI DISPLAY */

    // TODO

}


    // if all initialization processes were successful 
    return true;
}    
