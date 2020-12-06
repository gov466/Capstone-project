#include <SoftwareSerial.h>// Library for making a UART port

SoftwareSerial transmitter(10, 11); //RX & TX pin for UART Port

//Keeping Sensor 1 and Sensor 2 at a difference of 1 metre

int ir1 = 8; // Pin Number for Sensor 1
int ir2 = 9; // Pin Number for Sensor 2

long begin_, end_; // Variables to keep track of time
int difference; // Varibale to Store time taken by Car
float speed_,speed_in_ms,Acspeed_; //Variable to Store Calculated Speed of Car
int speedLim = 60; //Speed limit for Overspeeding

void setup() {
  
  Serial.begin(9600); //Initializing Serial Communications
  transmitter.begin(9600);

  pinMode(ir1,INPUT_PULLUP); //Making IR-1 pin as Input
  pinMode(ir2,INPUT_PULLUP); //Making IR-2 pin as Input


  Serial.println("STARTED");
  

}

void loop() 
{
  if(digitalRead(ir1)==LOW) //If Car detected in front of Sensor -1 
  {
    Serial.println("Car Detected at First Sensor"); //Print a message
    begin_ = millis(); //Record the Time when Car reaches sensor 1

    while(digitalRead(ir2)==HIGH) //Until Car is not detected at Sensor-2
    { 
      delay(1);//Waiting until Car Reaches Sensor 2
    }
     Serial.println("Car Detected at second Sensor"); //Print a message//Car reached at Sensor 2
    
    end_ = millis(); //Record the Time(in milliseconds) when Car reaches sensor 2

    difference = end_-begin_;//Calculate total time taken
    speed_in_ms = difference/1000.0; // Converting milliseconds to seconds
    Acspeed_ = 10/speed_in_ms; //s=d/t
    speed_ = (Acspeed_*3600)/1000.0; //Converting seconds to Hour and Km

    Serial.print("Speed of Car Detected: "); 
    Serial.print(int(speed_)); //printing speed
    Serial.println(" km/h"); //Print the calculated speed
    
   String data = ""; 
    data += String(int(speed_)); //typecasting to string
    //data += "";
    data.replace('4','z');  //replacing 4 in speed by z
     
    
    if(speed_ > speedLim) //If Detected Speed is more that Speed Limit
    {
      
      Serial.println("OVERSPEED DETECTED"); //Show Warning message
    }

    for(int i=0; i<3; i++)
    {
      transmitter.println(data); //transmitting data to bbb
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
  
      delay(1000); //1000 milliseconds
      String received;
      if (transmitter.available()) //reading back the trasnmitted data from bbb
      {
        Serial.print("GOT: ");
        while(transmitter.available()) 
        {
          received += (char)transmitter.read(); //recieving data from bbb
          delay(1);
        }
      }
      Serial.println(received); //printing the recievd data from bb to confrim that data trasnmission is successfull
      if(int(speed_) == received.toInt()) //acknowleding the transmitted data//.toint() coneverts string to integer
      {
        Serial.println("Data Transmitted Successfully"); //if statement comes true then print the msg
        break;
      }
      else
      {
        Serial.println("Data Transmission Failed. Retrying.."); //if statement is false then print this
      }
    }
  }
  else //If no data is being generated at Arduino side.At that time if any data comes from Beaglebone 
  //then it will be directly printed on Arduino Serial
  {
    if (transmitter.available()) 
    {
    Serial.write(transmitter.read());
    }
  }
}
