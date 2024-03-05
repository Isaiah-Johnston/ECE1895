#include "initialize.h"




bool initialize() {

/* SERIAL PORTS */
{ 
    // start serial communications
    Serial.begin(baud_rate);
    getSSerial().begin(baud_rate);

    // finished serial initialization
    Serial.println("INITIALIZATION STARTED...");                  delay(1);
    Serial.println("        ... beginning serial communication"); delay(1);
    Serial.println("SERIAL INITIALIZED");                         delay(1);
}


/* IO */
{  
    Serial.println("        ... resetting pins"); delay(1);
    
    // reset pins and error check
    if(!resetPins()) { return false; }
    
    Serial.println("        ... setting pin types"); delay(1);

    // set I/O types and error check 
    pinMode(power_SW,    INPUT_PULLUP ); if(!getPinMode(power_SW)   ) { return false; }
    pinMode(ignition_SW, INPUT_PULLUP ); if(!getPinMode(ignition_SW)) { return false; }
//  pinMode(gas_SW,      INPUT_PULLUP ); if(!getPinMode(gas_SW))      { return false; }
//  pinMode(brake_SW,    INPUT_PULLUP ); if(!getPinMode(brake_SW))    { return false; }
    pinMode(encoder_CLK, INPUT );        if(!getPinMode(encoder_CLK)) { return false; }
    pinMode(encoder_DT,  INPUT );        if(!getPinMode(encoder_DT) ) { return false; }

    // finished I/O initialization 
    Serial.println("I/O INITIALIZED"); delay(1);
}


/* ROTARY ENCODER */
{   
    Serial.println("        ... enabling encoder button debouncing"); delay(1);

    // set debouncing period for built-in encoder button
    encoder_button.setDebounceTime(50);

    Serial.println("        ... defining initial state for encoder"); delay(1);

    // define initial position for encoder
    encoder_prev_state = digitalRead(encoder_CLK);

    // finished rotary encoder initialization
    Serial.println("ROTARY ENCODER INITIALIZED"); delay(1);
}


/* MP3 PLAYER */
{ /*
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
*/}


/* BUTTONS  */
{ 
} // TODO


/* SWITCHES */
{ 
} // TODO


/* DISPLAY */
{ 
} // TODO


/* RTC */
{
} // TODO?


/* TASK PARAMETERS */
{
    // initialize task completion counter to zero
    curr_task                  = 10;
    prev_time                  = millis();
    prev_task                  = 10;  
    task_completion_count      = 0;
    task_completion_count_prev = 0;
    task_completion_goal       = 10;
    task_success_flag          = true;
    task_completion_time       = 5;

    Serial.println("        ... setting game score to 0");          delay(1);
    Serial.print("        ... setting game score goal to ");        delay(1);
    Serial.print(task_completion_goal);                             delay(1); 
    Serial.println();                                               delay(1);
    Serial.print("        ... setting time to complete tasks to "); delay(1);
    Serial.print(task_completion_time);                             delay(1);
    Serial.println();                                               delay(1);
    Serial.println("        ... setting previous task to 'NONE'");  delay(1);

    // finished task parameter initialization
    Serial.println("FINISHED TASK PARAMETER INITIALIZATION");       delay(1);
}


/* WAIT FOR GAME BEGIN */
{ 
    Serial.println("INITIALIZATION COMPLETE"); delay(1);
    Serial.println("        ... waiting for user"); delay(1);

    // wait for user to activate ignition switch
    while(!check_ignition()) { delayMicroseconds(1); } 

    // game begins when user activates ignition switch
    Serial.println("!!! GAME BEGIN !!!!"); delay(1);
}
   
   
    // if all initialization processes were successful 
    return true;
}    
