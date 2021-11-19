#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>


#define ONE_WIRE_BUS 5

Servo powerservo;  // hotplate power servo
Servo boilservo;  // hotplate boil button servo
Servo ramen_servo;
Servo shake;  // shaking servo
Servo pivot; // dispense servo

int incomingByte = 0; // for incoming serial data

int pos = 0;    // variable to store the servo position
uint16_t power_pos = 90;
uint16_t boil_pos = 270; // for the second button presser because the servo is oriented differently
uint16_t ramen_default_pos = 90;
uint16_t powder_pos = 0;

uint8_t ledPin =  3;
uint8_t start_button = 4;
uint8_t emergency_stop = 3;
uint8_t power_servo_pin = 9;
uint8_t boil_servo_pin = 10;
uint8_t ramen_servo_pin = 11;
uint8_t shake_pin = 12;
uint8_t pivot_pin = 13;
uint8_t sim_pin = 14;

int buttonStateStart = 0;
int buttonStateEnd = 0;


bool power = false;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(7,8,9,10,11,12);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  powerservo.attach(power_servo_pin);
  boilservo.attach(boil_servo_pin);
  ramen_servo.attach(ramen_servo_pin);
  
  powerservo.write(pos);
  boilservo.write(boil_pos);
  ramen_servo.write(ramen_default_pos);
  
  shake.attach(shake_pin);
  pivot.attach(pivot_pin);
  sensors.begin();

  pinMode(start_button, INPUT);
  pinMode(emergency_stop, INPUT);
  pinMode(ledPin, OUTPUT);
  
  int buttonStateStart = 0;
  int buttonStateEnd = 0;
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





void SIM900power()
{
  pinMode(sim_pin, OUTPUT); 
  digitalWrite(sim_pin,LOW);
  delay(1000);
  digitalWrite(sim_pin,HIGH);
  delay(2000);
  digitalWrite(sim_pin,LOW);
  delay(3000);
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

void reactToInput(int input){
  ramen_servo.write(90 - input);
  //0 is home position, 90 is arm extended and knocks the ramen out.
  //We can probably play around with speeds here to make the ramen fall
  //in a way that minimizes splash but that might not be v1
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

void powder_dispense(int repeat){
  for(int i = 0; i < repeat; i++){
      if (powder_pos == 0) {
            powder_pos = 180;
            pivot.write(powder_pos);
            delay(500);
            wiggle();
            Serial.println("current pos: 180");
     }
     else {
          powder_pos = 0;
          pivot.write(powder_pos);
          delay(500);
          wiggle();
          Serial.println("current pos: 0");
     }
    }
}

void ramen_dispense(){
    reactToInput(0);
    delay(500);
    reactToInput(90);
}

void lcd_display(String temp, String time_left){
  lcd.setCursor(0,0);
  delay(50);
  lcd.print("Temperature:  "+temp);
  delay(500);
  
  lcd.setCursor(0,1);
  delay(50);
  lcd.print("Time Left:  " + time_left);
  delay(500);
}


void run_all(){
  boil();
  while(true){
    float temp = temp_check();
    Serial.print(temp);
    Serial.print("C");
    Serial.println();
    Serial.print("Hot: ");
    Serial.print(power);
    Serial.println();
  
    if (temp > 95 && power){
      ramen_dispense();
      powder_dispense(4);
      delay(180000);
      hotplate_on_off();
    }
    buttonStateEnd = digitalRead(emergency_stop);
    Serial.print("End Status: ");
    Serial.print(buttonStateEnd);
    Serial.println();
    if (buttonStateEnd == HIGH){
      hotplate_on_off();
      digitalWrite(ledPin, LOW);
      Serial.println("Halted");
      break;
    }
    
  } 
  
}

void loop() {
  //lcd_display("95", "8:30");
  //ramen_dispense();
  //powder_dispense(4);
  buttonStateStart = digitalRead(start_button);
  if (buttonStateStart == HIGH) {
    digitalWrite(ledPin, HIGH);
    Serial.print("Start Status: ");
    Serial.print(buttonStateStart);
    Serial.println();
    run_all();
  } 
}
