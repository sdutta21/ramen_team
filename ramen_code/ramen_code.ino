#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5

Servo powerservo;  // hotplate power servo
Servo boilservo;  // hotplate boil button servo
Servo ramen_servo;

int incomingByte = 0; // for incoming serial data


int pos = 0;    // variable to store the servo position
uint16_t power_pos = 90;
uint16_t boil_pos = 270; // for the second button presser because the servo is oriented differently
uint16_t ramen_default_pos = 0;

uint8_t power_servo_pin = 9;
uint8_t boil_servo_pin = 10;
uint8_t ramen_servo_pin = 11;

bool power = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  powerservo.attach(power_servo_pin);
  boilservo.attach(boil_servo_pin);
  ramen_servo.attach(ramen_servo_pin);
  
  powerservo.write(pos);
  boilservo.write(boil_pos);
  ramen_servo.write(ramen_default_pos);
  sensors.begin();
}

//toggle hotplate switch
void hotplate_on_off(){
  powerservo.write(power_pos);
  delay(500);
  powerservo.write(pos);
  
  if (power){
    power = false;
  }
  else{
    power = true;
  }
}

//toggle boil switch
void hotplate_boil(){
  boilservo.write(pos);
  delay(500);
  boilservo.write(boil_pos);
}

void boil(){
  hotplate_on_off();
  delay(500);
  hotplate_boil();
  delay(1000);
  hotplate_boil();
  delay(1000);
}

float temp_check(){
  sensors.requestTemperatures(); 
  delay(1000);
  float celcius = sensors.getTempCByIndex(0);
  return celcius;
}

int reactToInput(int input){
  panServo.write(90 - input);
  //0 is home position, 90 is arm extended and knocks the ramen out.
  //We can probably play around with speeds here to make the ramen fall
  //in a way that minimizes splash but that might not be v1
  return input;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.println(incomingByte);
    if (incomingByte == 10){
      boil();
    }
    else if(incomingByte == 50){
      hotplate_on_off();
    }
  }
  
  float temp = temp_check();
  Serial.print(temp);
  Serial.print("C");
  Serial.println();
  Serial.println(power);
  if (temp > 95 && power){
    reactToInput(80);
    sleep(500);
    reactToInput(0);
    sleep(180000);
    hotplate_on_off();
  }
  
}
