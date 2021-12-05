#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>


#define ONE_WIRE_BUS 5

Servo powerservo;  // hotplate power servo
Servo boilservo;  // hotplate boil button servo
Servo ramen_servo;
Servo shake;  // shaking servo
Servo pivot; // dispense servo
String number;
String Data;

int incomingByte = 0; // for incoming serial data

int pos = 0;    // variable to store the servo position
uint16_t power_pos = 90;
uint16_t boil_pos = 270; // for the second button presser because the servo is oriented differently
uint16_t ramen_default_pos = 90;
uint16_t powder_pos = 0;

uint8_t ledPin =  2;
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

uint8_t minutes = 6;
uint8_t secondes = 0;
char timeline[16];
char lcd_temp[16];

bool power = false;


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal lcd(22,24,26,28,30,32);
SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Arduino #7 & #8



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Initializing..."); 
  
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

  lcd.begin(16, 2);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
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
      
      while(secondes != 0 && minutes != 0){
        sprintf(lcd_temp, "%f", temp);
        sprintf(timeline,"%0.2d mins %0.2d secs", minutes, secondes);
        lcd_display(lcd_temp,timeline);
        
        delay(1000);
        secondes--;
        if (secondes == 0)
        {
          secondes = 59;
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
      digitalWrite(ledPin, LOW);
      Serial.println("Halted");
      break;
    }
    
  } 
  
}

void SendMessage(String receiver, String message){
  Serial.println("SendMessage");
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  String number = "\""  + receiver + "\"";
  Serial.println("AT+CMGS=" + number);
  updateSerial();
  mySerial.println("AT+CMGS=" + number);
  updateSerial();
  mySerial.print(message); //text content
  updateSerial();
  mySerial.write(26);
}

void loop() {

  //SMS Version
  updateSerial();
  if(number != NULL){
    SendMessage(number, "Preparing Ramen..."); 
    digitalWrite(ledPin, HIGH);
    run_all();
    SendMessage(number, "Ramen Done."); 
    number = "";
  }

  //Button Version
  
//  buttonStateStart = digitalRead(start_button);
//  if (buttonStateStart == HIGH) {
//    digitalWrite(ledPin, HIGH);
//    Serial.print("Start Status: ");
//    Serial.print(buttonStateStart);
//    Serial.println();
//    run_all();
//  } 
}
