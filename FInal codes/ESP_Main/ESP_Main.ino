#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

char ssid[] = "572pharmacy";   // your network SSID (name) 
char pass[] = "572pharmacY001";   // your network password
WiFiClient  client;

unsigned long myChannelNumber = 1239710;
const char * myWriteAPIKey = "K9N399Q6EPCKDTTH";

long oldmillis;

int speedx = -1;

void setup() {
  Serial.begin(9600);  //Initialize serial

  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  oldmillis = millis();
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  if(Serial.available())
  {
    speedx  = Serial.parseInt();
    Serial.println(speedx);
  }

if(speedx != -1 and (millis()-oldmillis)>16000)
{

  ThingSpeak.setField(1, 1);
  ThingSpeak.setField(2, speedx );

  oldmillis = millis();

  Serial.print("Uploading Data: ");
  Serial.println(speedx);
  
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    ;
    //Serial.println("Channel update successful.");
  }
  else{
    
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
 }
delay(100);
speedx = -1;
}
