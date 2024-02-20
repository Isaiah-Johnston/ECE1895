int isPressed = 0;

void setup() {

  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, INPUT);

}

void loop() {

  isPressed = digitalRead(8);

  if (isPressed == LOW) {

    digitalWrite(5, HIGH); 
    delay(1000);                     
    digitalWrite(5, LOW);   
    digitalWrite(6, HIGH); 
    delay(1000);                     
    digitalWrite(6, LOW);   
    digitalWrite(7,HIGH);
    delay(1000);
    digitalWrite(7, LOW);   
    delay(500);
  }
}
