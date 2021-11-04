/*
  Multiple Serial test

  Receives from the main serial port, sends to the others.
  Receives from serial port 1, sends to the main serial (Serial 0).

  This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc.

  The circuit:
  - any serial device attached to Serial port 1
  - Serial Monitor open on Serial port 0

  created 30 Dec 2008
  modified 20 May 2012
  by Tom Igoe & Jed Roach
  modified 27 Nov 2015
  by Arturo Guadalupi

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/MultiSerialMega
*/

#include <Servo.h>

Servo shake;  // create servo object to control a servo
Servo pivot;
// twelve servo objects can be created on most boards

int pos = 0;

void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  pivot.attach(9);
  shake.attach(10);
}

void loop() {
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
     s.trim();
    Serial.print(s);
    if (s == "l") {
      Serial.println("Works!");
      Serial.println("wiggled");
      
         if (pos == 0) {
            pos = 180;
            pivot.write(pos);
            delay(500);
            wiggle();
            Serial.println("current pos: 180");

         }
         else {
          pos = 0;
          pivot.write(pos);
          delay(500);
          wiggle();
          Serial.println("current pos: 0");


         }

    }
     // Serial.println(Serial.readStringUntil("\n"));
  }
  
 /* if (Serial.available()) {
    int s = Serial.read();
    Serial.println("works!");
    if (s == 1){
       if (pos == 0){
        pos = 180;
      }
      else {
        pos = 0;
      }
    }
  }*/
}
void wiggle() {
  for (int i = 0; i < 5; i++) {
          shake.write(90);
          delay(200);
          shake.write(180);
          delay(200);
          shake.write(90);
          delay(200);
  }

}
