/*
  Task 3.1
  Reading a light dependent resistor.

  Code based on AnalogReadSerial Arduino example:
  
  "AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain."
  
*/
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // map input to the frequency of a potential oscillator
  int frequency = map(sensorValue, 0, 1023, 220, 880);
  // print out the value you read:
  Serial.println(sensorValue); 
  Serial.println(frequency);
  delay(1000);        // delay in between reads for stability
  
}
