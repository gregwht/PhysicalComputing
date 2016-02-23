/* 
 LED Dimmer code from http://www.toptechboy.com/arduino/lesson-11-arduino-circuit-to-dim-led-with-potentiometer/
 
 Servo code from Scott Fitzgerald http://arduino.cc/en/Tutorial/Knob
*/

#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
 
int potpin = 0;  // analog pin used to connect the potentiometer
int LEDPin = 3;  // LED pin
int readValue;   // Reading pot
int serValue;     // Writing to servo
int LEDValue;   // Writing to LED

 
void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  Serial.begin(9600);
} 
 
void loop() 
{ 
  readValue = analogRead(potpin);              // reads the value of the potentiometer (value between 0 and 1023) 
  serValue = map(readValue, 0, 1023, 0, 180);  // scale it to use it with the servo (value between 0 and 180) 
  myservo.write(serValue);                     // sets the servo position according to the scaled value 
  delay(15);                                   // waits for the servo to get there 

  LEDValue = (255./1023.) * readValue;          // calculate LED value
  analogWrite(LEDPin, LEDValue);               // write to LED

} 
