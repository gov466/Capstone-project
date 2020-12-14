#include <SoftwareSerial.h>// Library for making a UART port

SoftwareSerial transmitter(10, 11); //RX & TX pin for UART Port

//Keeping Sensor 1 and Sensor 2 at a difference of 1 metre

int ir1 = 8; // Pin Number for Sensor 1
int ir2 = 9; // Pin Number for Sensor 2

long begin, end; // Variables to keep track of time
int difference; // Varibale to Store time taken by Car
float speed; //Variable to Store Calculated Speed of Car
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
    begin = millis(); //Record the Time when Car reaches sensor 1

    while(digitalRead(ir2)==HIGH) //Until Car is not detected at Sensor-2
    { 
      delay(1);//Waiting until Car Reaches Sensor 2
    }
    //Car reached at Sensor 2
    
    end = millis(); //Record the Time when Car reaches sensor 2

    difference = end-begin;//Calculate total time taken
    speed = difference/1000.0; // Converting milliseconds to seconds
    speed = 1.0/speed;
    speed = (speed*3600)/1000.0; //Converting seconds to Hour and Km

    Serial.print("Speed of Car Detected: "); 
    Serial.print(int(speed));
    Serial.println(" km/h"); //Print the calculated speed
    
    String data = "";
    data += String(int(speed));
    data += "";
    data.replace('4','z');
    
    
    if(speed > speedLim) //If Detected Speed is more that Speed Limit
    {
      
      Serial.println("OVERSPEED DETECTED"); //Show Warning message
    }

    for(int i=0; i<3; i++)
    {
      transmitter.println(data);
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
      transmitter.println(" ");
  
      delay(1000);
      String received;
      if (transmitter.available()) 
      {
        Serial.print("GOT: ");
        while(transmitter.available())
        {
          received += (char)transmitter.read();
          delay(1);
        }
      }
  
      Serial.println(received);
  
      if(int(speed) == received.toInt())
      {
        Serial.println("Data Transmitted Successfully");
        break;
      }
      else
      {
        Serial.println("Data Transmission Failed. Retrying..");
      }
  
    }


    
    

    
  }
  else
  {
    if (transmitter.available()) 
    {
    Serial.write(transmitter.read());
    }
  }
  
  

}
