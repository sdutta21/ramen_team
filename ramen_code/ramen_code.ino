#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 5

Servo powerservo;  // hotplate power servo
Servo boilservo;  // hotplate boil button servo

int incomingByte = 0; // for incoming serial data


int pos = 0;    // variable to store the servo position
uint16_t power_pos = 90;
uint16_t boil_pos = 270; // for the second button presser because the servo is oriented differently

uint8_t power_servo_pin = 9;
uint8_t boil_servo_pin = 10;

bool power = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  powerservo.attach(power_servo_pin);
  boilservo.attach(boil_servo_pin);
  powerservo.write(pos);
  boilservo.write(boil_pos);
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
    hotplate_on_off();
  }
  
}
