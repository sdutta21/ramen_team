#include <SpeedyStepper.h>


const int MOTOR_STEP_PIN = 35;
const int MOTOR_DIRECTION_PIN = 37;


//
// create the stepper motor object
//
SpeedyStepper stepper;



void setup() 
{
  //

   
  Serial.begin(9600);


  //
  // connect and configure the stepper motor to its IO pins
  //
  stepper.connectToPins(35, 37);
}



void loop() 
{

  stepper.setSpeedInStepsPerSecond(300);
  stepper.setAccelerationInStepsPerSecondPerSecond(300);


  stepper.moveRelativeInSteps(500);


  delay(1000);

  stepper.moveRelativeInSteps(-500);
  delay(1000);

  
}
