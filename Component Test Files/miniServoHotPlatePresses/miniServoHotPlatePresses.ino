
#include <Servo.h>

Servo powerservo;  // create servo object to control a servo
Servo boilservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

int angle = 90;
int twoangle = 270; // for the second button presser because the servo is oriented differently

void setup() {
  // put your setup code here, to run once:
 powerservo.attach(9);  // attaches the servo on pin 9 to the servo object
 boilservo.attach(10);  // attaches the servo on pin 10 to the servo object
}

//OKay so this whole loop just swings each servo arm down. If its off, the hotplate will turn on, if its on, the hotplate will turn off.  
void loop() {
  // put your main code here, to run repeatedly:
 for (pos = 0; pos <= angle; pos += angle) { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
    powerservo.write(pos);              // tell servo to go to position in variable 'pos'
                    
  }
   delay(1000);
  for (pos = angle; pos >= 0; pos -= angle) { // goes from 90 degrees to 0 degrees
    powerservo.write(pos);              // tell servo to go to position in variable 'pos'
                  
  }
 for (pos = 0; pos <= twoangle; pos -= angle) { // goes from 0 degrees to 270 degrees
    // in steps of 1 degree
    boilservo.write(pos);              // tell servo to go to position in variable 'pos'
                   
  }
   delay(1000);
  for (pos = twoangle; pos >= 0; pos += angle) { // goes from 270 degrees to 0 degrees
    boilservo.write(pos);              // tell servo to go to position in variable 'pos'
                
  }
     delay(1000);
}
