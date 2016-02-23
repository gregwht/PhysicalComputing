/*
  Blink code by Scott Fitzgerald
  Modified by Gregory White
  02/02/16
 */


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(4, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  
  int rand = random(0,299);
  int pin;
  
  if (rand >= 0 && rand <= 99){
    pin = 13;
  }
  else if (rand >= 100 && rand <= 199){
    pin = 12;
  }
  else if (rand >= 200 && rand <= 299){
   pin = 4; 
  }
  
  Serial.println(rand);
  
  digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);              // wait for a second
  digitalWrite(pin, LOW);    // turn the LED off by making the voltage LOW
  delay(200);              // wait for a second
}
