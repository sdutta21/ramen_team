// include the Servo library
#include <Servo.h>

Servo panServo;  // create a servo object
Servo tiltServo;  // create a servo object

const int distancePin = A0; // analog pin to read the sensor input

int sensorValue;
int distance;
int panAngle;
int tiltAngle;
int directionFlag;
int stepSize;
int minPan;
int maxPan;
int minTilt;
int maxTilt;
int sensorValue1;
int sensorValue2;
int sensorValue3;
double avgSensorValue;

void setup() {
  tiltServo.attach(9); // attaches the servo on pin 9 to the servo object
  panServo.attach(10); // attaches the servo on pin 10 to the servo object
  Serial.begin(9600); // open a serial connection to your computer
  
  
}

void loop() {

    if (Serial.available()){
     String s = Serial.readStringUntil('\n');
     s.trim();
     int returnMessage = reactToInput(s.toInt());
     Serial.print("Angle set to: ");
     Serial.println(returnMessage);
  }

  
}


int reactToInput(int input){
  panServo.write(90 - input);
  //0 is home position, 90 is arm extended and knocks the ramen out.
  //We can probably play around with speeds here to make the ramen fall
  //in a way that minimizes splash but that might not be v1
  return input;
}
