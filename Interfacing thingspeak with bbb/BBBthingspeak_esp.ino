

#include "ThingSpeak.h"
#include <ESP8266WiFi.h>

char ssid[] = "572pharmacy";   // your network SSID (name) 
char pass[] = "572pharmacY001";   // your network password
WiFiClient  client;

unsigned long myChannelNumber = 1239710;
const char * myWriteAPIKey = "K9N399Q6EPCKDTTH";

// Initialize our values
int number1 = -1;


void setup() {
  Serial.begin(9600);  // Initialize serial
  pinMode(D0,OUTPUT);
  digitalWrite(D0,LOW);

  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak

  Serial.println();

  WiFi.begin(ssid, pass); //begin wifi connection
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected."); //if everything is OK print statement
  
}

void loop() {

 

  if(Serial.available()) //checking if serial is availalble
  {
   while(Serial.available())
   {
    number1 = Serial.parseInt();  //storing the recieved data to number1
    Serial.print("GOT : ");
    Serial.println(number1); //serial print
   }
  }

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    while(WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(10000);     
    } 
    Serial.println();
    Serial.println("\nConnected.");
  }

  if(number1 >= 0) //checking if recived number is greater than zero
  {
      digitalWrite(D0,HIGH); //inbuilt led goes high
      // set the fields with the values
      Serial.print("Uploading : ");
      Serial.println(number1);
      ThingSpeak.setField(1, number1); //field #1 and data
      
      // write to the ThingSpeak channel
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if(x == 200) //HTML code for update success
      {
        Serial.println("Channel update successful.");
      }
      else
      {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    
      
      delay(20000); // Wait 20 seconds to update the channel again
      digitalWrite(D0,LOW); 
  }
  delay(100);
}
