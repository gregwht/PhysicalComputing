/*
  SOS Blink
 Repeatedly flashes SOS in morse code.
 
 Based on the Arduino Blink code by Scott Fitzgerald.
 
 */

boolean sign; // 0 = dot, 1 = dash
int pattern[9]; // pattern of dots and dashes


void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
  
  sign = 0;
  
  pattern[0] = 0;
  pattern[1] = 0;
  pattern[2] = 0;
  pattern[3] = 1;
  pattern[4] = 1; 
  pattern[5] = 1;
  pattern[6] = 0;
  pattern[7] = 0;
  pattern[8] = 0;  
}

// the loop function runs over and over again forever
void loop() {
 
  for (int i = 0; i<9; i++){
   
    sign = pattern[i];
    
    if (sign){dash();}
    if (!sign){dot();}
  }
  
  delay(1000);              // wait for a second

}

void dot() {
 
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(100);              // wait for a second
}

void dash() {
   
  //delay(200);
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(400);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(100);              // wait for a second
}

