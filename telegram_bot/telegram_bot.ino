#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h> 
#include <TelegramBot.h> 


// Initialize Wifi connection to the router 
const char* ssid     = "OLIN-DEVICES"; 
const char* password = "Design&Fabric8"; 

// Initialize Telegram BOT 
const char BotToken[] = "5038312336:AAFXYozRkq-XTGOT4TOrh2z-3z4tcOaeR8Q"; 
WiFiClientSecure net_ssl; 
TelegramBot bot (BotToken, net_ssl); 

// the number of the LED pin   
void setup()  
{   
    Serial.begin(115200);   
    while (!Serial) {}  //Start running when the serial is open  
    delay(3000);   
    // attempt to connect to Wifi network:   
    Serial.print("Connecting Wifi: ");   
    Serial.println(ssid);   
      while (WiFi.begin(ssid, password) != WL_CONNECTED)  
        {   
    Serial.print(".");   
    delay(500);   
        }   
        
        Serial.println("");   
        Serial.println("WiFi connected");   
        bot.begin();   
}   

void loop()  
{   
message m = bot.getUpdates(); // Read new messages   
if (m.text.equals("on"))  
      {   
     
  bot.sendMessage(m.chat_id, "The Led is now ON");   
}   
else if (m.text.equals("off"))  
      {   
  bot.sendMessage(m.chat_id, "The Led is now OFF");   
}   
}  
