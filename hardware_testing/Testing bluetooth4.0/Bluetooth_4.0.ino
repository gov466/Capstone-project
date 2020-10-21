#include <SoftwareSerial.h> //library to have serial 
//communication over digital pins
SoftwareSerial mySerial(2,3);
int ledpin=13; //connected led pin to pin 13
void setup()
{
mySerial.begin(9600); //open serial port
Serial.begin(9600); //begin serial communication
pinMode(ledpin,OUTPUT); //setting led as output
}
void loop()
{
int i; //declaring a variable
if (mySerial.available()) 
{
i=mySerial.read(); // reading the data recieved at serial port
Serial.println("DATA RECEIVED:"); //printing the data in serial monitor
if(i=='1') //if recieved data is one 
{
digitalWrite(ledpin,1); //turn on led connected to pin 13
Serial.println("led on"); //print serial message
}
if(i=='0') //if recieved data is zero turn off the led.
{
digitalWrite(ledpin,0);
Serial.println("led off");
}
}
}
