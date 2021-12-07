#include <Servo.h>

Servo myservo;  // create servo object to control a servo

void setup() {
  myservo.attach(50);  // attaches the servo on pin 9 to the servo object
}

void open_water(){
  myservo.write(15);                  // sets the servo position according to the scaled value
  delay(15);
}

void close_water(){
  myservo.write(0);                  // sets the servo position according to the scaled value
  delay(15);
}

void loop() {
  open_water();
  close_water();
}
