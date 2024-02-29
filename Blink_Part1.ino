// This is Isaiah's Branch
// Testing Testing 123 123

int isPressed = 0;

void setup() {
// Alex's comment
  pinMode(7, OUTPUT);
  pinMode(8, INPUT);
}

void loop() {
//This is Dillon Hepler's branch
  isPressed = digitalRead(8);

  if (isPressed == LOW) {

    digitalWrite(7, HIGH);  
    delay(1000);                    
    digitalWrite(7, LOW);  
    delay(1000);                      
  }
}
