#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <SpeedyStepper.h>

//Servos
Servo power_servo;  // hotplate power servo
Servo boil_servo;  // hotplate boil button servo
Servo ramen_servo;
Servo pivot_servo; // dispense servo
Servo water_servo; //water dispense servo
Servo temp_servo;

//servo positions
int power_default_pos = 0;    // variable to store the servo position
int power_pos = 90;
int boil_pos = 270; // for the second button presser because the servo is oriented differently
int ramen_default_pos = 90;
int powder_pos = 65;
int powder_default_pos = 155;
int water_default_pos = 180;
int water_pos = 0;
int temp_default_pos = 110;
int temp_pos = 10;

//pins
uint8_t start_button = 34;
uint8_t emergency_stop = 36;
uint8_t power_servo_pin = 6;
uint8_t boil_servo_pin = 5;
uint8_t ramen_servo_pin = 10;
uint8_t shake_pin = 7;
uint8_t pivot_servo_pin = 11;
uint8_t water_servo_pin = 4;
uint8_t temp_sensor_pin = 38;
uint8_t temp_servo_pin = 3;
uint8_t stepper_step_pin = 35;
uint8_t stepper_direction_pin = 37;
int limit_switch = A0;
uint8_t cook_pin = 1;

//states

int buttonStateStart = 0;
int buttonStateEnd = 0;
int telegram_start = 0;
int minutes = 3;
int seconds = 59;
char timeline[16];
char lcd_temp[16];
bool power = false;

//Initalization

OneWire oneWire(temp_sensor_pin);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(22,24,26,28,30,32);
SpeedyStepper stepper;

void setup() {
  Serial.begin(9600);
  
  power_servo.attach(power_servo_pin);
  boil_servo.attach(boil_servo_pin);
  ramen_servo.attach(ramen_servo_pin);
  water_servo.attach(water_servo_pin);
  pivot_servo.attach(pivot_servo_pin);
  temp_servo.attach(temp_servo_pin);
    
  power_servo.write(power_default_pos);
  boil_servo.write(boil_pos);
  ramen_servo.write(ramen_default_pos);
  water_servo.write(water_default_pos);
  temp_servo.write(temp_default_pos);
  pivot_servo.write(powder_default_pos);

  stepper.connectToPins(stepper_step_pin, stepper_direction_pin);
  sensors.begin();

  pinMode(start_button, INPUT);
  pinMode(emergency_stop, INPUT);
  pinMode(shake_pin, OUTPUT);
  pinMode(limit_switch, INPUT);
  pinMode(cook_pin, INPUT);

  int buttonStateStart = 0;
  int buttonStateEnd = 0;

  lcd.begin(16, 2);
}

//toggle hotplate switch
void hotplate_on_off(){
  power_servo.write(power_pos);
  delay(500);
  power_servo.write(power_default_pos);
  
  if (power){
    power = false;
  }
  else{
    power = true;
  }
}

//toggle boil switch
void hotplate_boil(){
  boil_servo.write(power_default_pos);
  delay(500);
  boil_servo.write(boil_pos);
}

void boil(){
  hotplate_on_off();
  delay(500);
  hotplate_boil();
  delay(1000);
  hotplate_boil();
  delay(1000);
}

void open_water(){
  water_servo.write(water_pos);                  // sets the servo position according to the scaled value
  delay(1000);
}

void close_water(){
  water_servo.write(water_default_pos);                  // sets the servo position according to the scaled value
  delay(1000);
}

void dispense_water(){
  open_water();
  delay(50000);
  close_water();
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

void ramen_dispense(){
    reactToInput(0);
    delay(500);
    reactToInput(-90);
    delay(500);
    reactToInput(0);
}

void vibrate() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(shake_pin, HIGH);
    delay(500);
    digitalWrite(shake_pin, LOW);
  }
}

void powder_dispense(int repeat){
  for(int i = 0; i < repeat; i++){
      pivot_servo.write(powder_pos);
      delay(1000);
      pivot_servo.write(powder_default_pos);
      delay(1000);
    }
}

void lcd_display(String temp, String time_left){
  lcd.setCursor(0,0);
  lcd.print("Temperature:  "+ temp);
  
  lcd.setCursor(0,1);
  lcd.print("Time Left:  " + time_left);
}

void stepper_ramen(){
  stepper.setSpeedInStepsPerSecond(300);
  stepper.setAccelerationInStepsPerSecondPerSecond(300);

  while(analogRead(limit_switch) == 0){
    Serial.println(analogRead(limit_switch));
    stepper.moveRelativeInSteps(-500);
    if (analogRead(limit_switch) != 0){ 
      break;
    }
  }
  stepper.moveRelativeInSteps(200); // full rev backwards
  ramen_dispense();
}


void run_all(){
  dispense_water();
  delay(3000);
  temp_servo.write(temp_pos);
  delay(1000);
  boil();
  delay(500);
  while(true){
    float temp = temp_check();
    Serial.print(temp);
    Serial.print("C");
    Serial.println();
    Serial.print("Hot: ");
    Serial.print(power);
    Serial.println();
    
    String str_temp = String(temp, 2);
    Serial.println(str_temp);
    lcd_display(str_temp,"?");
    
    
    if (temp > 90 && power){
      temp_servo.write(temp_default_pos);
      delay(500);
      powder_dispense(4);
      delay(5000);
      stepper_ramen();
      delay(1000);
      
      
      while(seconds != 0 || minutes != 0){
        sprintf(lcd_temp, "%d", temp);
        sprintf(timeline,"%0.1d:%0.2d", minutes, seconds);
        lcd_display(lcd_temp,timeline);
        
        delay(1000);
        seconds--;
        if (seconds == 0)
        {
          seconds = 59;
          minutes --;
        }
        if(minutes < 0){
          break;
        }
      }
      hotplate_on_off();
      
    }
    
    
    buttonStateEnd = digitalRead(emergency_stop);
    Serial.print("End Status: ");
    Serial.print(buttonStateEnd);
    Serial.println();
    if (buttonStateEnd == HIGH){
      hotplate_on_off();
      stepper.moveRelativeInSteps(0);
      temp_servo.write(temp_default_pos);
      Serial.println("Halted");
      break;
    }
    
  } 
  
}

void loop() {
//  telegram_start = digitalRead(cook_pin);
//  if (telegram_start == HIGH) {
//    Serial.print("Start Status: ");
//    Serial.print(buttonStateStart);
//    Serial.println();
//    run_all();
//  } 


  buttonStateStart = digitalRead(start_button);
  if (buttonStateStart == HIGH) {
    //digitalWrite(esp8266, LOW);
    Serial.print("Start Status: ");
    Serial.print(buttonStateStart);
    Serial.println();
    run_all();
  } 
}
