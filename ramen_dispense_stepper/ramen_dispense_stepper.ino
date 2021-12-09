#include <SpeedyStepper.h>


const int MOTOR_STEP_PIN = 35;
const int MOTOR_DIRECTION_PIN = 37;

int limit_switch = A0;

SpeedyStepper stepper;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(limit_switch, INPUT);
  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
}

void stepper_ramen(){
  stepper.setSpeedInStepsPerSecond(300);
  stepper.setAccelerationInStepsPerSecondPerSecond(300);

  while(analogRead(limit_switch) == 0){
    Serial.println(analogRead(limit_switch));
    stepper.moveRelativeInSteps(-500);
    if (analogRead(limit_switch) != 0){
      stepper.moveRelativeInSteps(100); // full rev backwards
      break;
    }
    //delay(500);
  }
}

void reset_stepper(){
  stepper.setSpeedInStepsPerSecond(300);
  stepper.setAccelerationInStepsPerSecondPerSecond(300);
  stepper.moveRelativeInSteps(-500);
}


void loop() {

  stepper_ramen();
  reset_stepper();

}
