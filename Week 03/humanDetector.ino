/* 
 LED Dimmer code from http://www.toptechboy.com/arduino/lesson-11-arduino-circuit-to-dim-led-with-potentiometer/
 
 Servo code from Scott Fitzgerald http://arduino.cc/en/Tutorial/Knob

 Human detection code from Gregory White.

*/

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
 
int serPin = 11;  // servo
int LEDPin = 10;  // LED
int piePin = 9;   // piezo
int butPin = 8;   // button
int LDRPin = 0;   // analog pin used to connect the potentiometer

int serValue;    // Writing to servo
int LEDValue;    // Writing to LED
int pieValue;    // Writing to piezo
int readValue;   // Reading LDR

int butValue = 0;// Set button to off
 
void setup(){
  
  myservo.attach(11);  // attaches the servo on pin 9 to the servo object 
  Serial.begin(9600);
  
  pinMode(LEDPin, OUTPUT);
  pinMode(piePin, OUTPUT);
  pinMode(butPin, INPUT);
} 
 
void loop(){ 
  
  butValue = digitalRead(butPin);
  
  if (butValue == LOW){
  
    readValue = analogRead(LDRPin);              // reads the value of the potentiometer (value between 0 and 1023) 
    serValue = map(readValue, 0, 1023, 0, 180);  // scale it to use it with the servo (value between 0 and 180) 
    myservo.write(serValue);                     // sets the servo position according to the scaled value 
    delay(15);                                   // waits for the servo to get there 
    
    digitalWrite(LEDPin, LOW);
    digitalWrite(piePin, LOW);
  }
  
  else {
   
    serValue = 180;
    myservo.write(serValue);
    delay(15);
    
    digitalWrite(LEDPin, HIGH);
    digitalWrite(piePin, HIGH);
    delayMicroseconds(56);
    digitalWrite(piePin, LOW);
    delayMicroseconds(56);
    
  }
} 
