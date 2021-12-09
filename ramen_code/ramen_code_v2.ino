#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

//Servos
Servo power_servo;  // hotplate power servo
Servo boil_servo;  // hotplate boil button servo
Servo ramen_servo;
Servo pivot_servo; // dispense servo
Servo water_servo; //water dispense servo

//servo positions
int power_default_pos = 0;    // variable to store the servo position
int power_pos = 90;
int boil_pos = 270; // for the second button presser because the servo is oriented differently
int ramen_default_pos = 90;
int powder_pos = 0;
int water_default_pos = 0;

//pins
uint8_t start_button = 34;
uint8_t emergency_stop = 36;
uint8_t power_servo_pin = 40;
uint8_t boil_servo_pin = 42;
uint8_t ramen_servo_pin = 52;
uint8_t shake_pin = 44;
uint8_t pivot_servo_pin = 53;
uint8_t water_servo_pin = 50;
uint8_t temp_sensor_pin = 38;

//states

int buttonStateStart = 0;
int buttonStateEnd = 0;
int minutes = 6;
int seconds = 0;
char timeline[16];
char lcd_temp[16];
bool power = false;

//Initalization

OneWire oneWire(temp_sensor_pin);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(22,24,26,28,30,32);

void setup() {
  Serial.begin(9600);
  
  power_servo.attach(power_servo_pin);
  boil_servo.attach(boil_servo_pin);
  ramen_servo.attach(ramen_servo_pin);
  water_servo.attach(water_servo_pin);
  pivot_servo.attach(pivot_servo_pin);

  power_servo.write(power_default_pos);
  boil_servo.write(boil_pos);
  ramen_servo.write(ramen_default_pos);
  water_servo.write(water_default_pos);
  
  sensors.begin();

  pinMode(start_button, INPUT);
  pinMode(emergency_stop, INPUT);
  pinMode(shake, OUTPUT);

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

void open_water(){
  water_servo.write(15);                  // sets the servo position according to the scaled value
  delay(15);
}

void close_water(){
  water_servo.write(0);                  // sets the servo position according to the scaled value
  delay(15);
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
    reactToInput(90);
}

void vibrate() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(shake, HIGH);
    delay(500);
    digitalWrite(shake, LOW);
  }
}

void powder_dispense(int repeat){
  for(int i = 0; i < repeat; i++){
      if (powder_pos == 0) {
            powder_pos = 180;
            pivot.write(powder_pos);
            delay(500);
            vibrate();
            Serial.println("current pos: 180");
     }
     else {
          powder_pos = 0;
          pivot.write(powder_pos);
          delay(500);
          vibrate();
          Serial.println("current pos: 0");
     }
    }
}

void lcd_display(String temp, String time_left){
  lcd.setCursor(0,0);
  delay(50);
  lcd.print("Temperature:  "+ temp);
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
      
      while(seconds != 0 && minutes != 0){
        sprintf(lcd_temp, "%f", temp);
        sprintf(timeline,"%0.2d mins %0.2d secs", minutes, seconds);
        lcd_display(lcd_temp,timeline);
        
        delay(1000);
        seconds--;
        if (seconds == 0)
        {
          seconds = 59;
          minutes --;
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
      Serial.println("Halted");
      break;
    }
    
  } 
  
}

void loop() {

  //SMS Version
//  updateSerial();
//  if(number != NULL){
//    SendMessage(number, "Preparing Ramen..."); 
//    digitalWrite(ledPin, HIGH);
//    run_all();
//    SendMessage(number, "Ramen Done."); 
//    number = "";
//  }

  //Button Version
  float temp = temp_check();
  Serial.println(temp);
//  buttonStateStart = digitalRead(start_button);
//  if (buttonStateStart == HIGH) {
//    digitalWrite(ledPin, HIGH);
//    Serial.print("Start Status: ");
//    Serial.print(buttonStateStart);
//    Serial.println();
//    run_all();
//  } 
}