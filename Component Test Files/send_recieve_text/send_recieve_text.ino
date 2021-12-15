#include <SoftwareSerial.h>

//Create software serial object to communicate with SIM900
SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Arduino #7 & #8
String number;
String Data;
void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  
  //Begin serial communication with Arduino and SIM900
  mySerial.begin(9600);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
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

void loop()
{

  updateSerial();
  if(number != NULL){
    SendMessage(number, "Preparing Ramen..."); 
    
    delay(10000);
    SendMessage(number, "Ramen Done."); 
    number = "";
  }
 
}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {

    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port

  }
  while(mySerial.available()) 
  {
    char character = mySerial.read(); // Receive a single character from the software serial port
    Data.concat(character); // Add the received character to the receive buffer

    Serial.write(character);
    
    if (character == '\n'){

      if(Data.indexOf("+CMT") == 0){
        
        char subbuff[13];
        memcpy(subbuff, &Data[7], 12);
        subbuff[12] = '\0';
        number = subbuff;
        }  
        
        Data = "";
     }
    
  }
}
