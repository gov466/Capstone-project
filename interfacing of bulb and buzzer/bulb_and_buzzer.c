#include <iobb.h>                             // input/output library for Beaglebone black
#include <stdio.h>                            // standarrd input/output library

int bulbPin = 30;                             //decalring port 9 pin 30 for bulb output
int port9 = 9;                                //port 9
int buzzerPin = 8;                            //declaring port 8 pin 8 as buzzer output
int port8 = 8;                                //port8


int speed = 30;                               //speed of vehicle

int threshold = 20;                           //maximum speed

int main(void)  
{  
  iolib_init();                                //iolib initialization
  iolib_setdir(port8, buzzerPin, DigitalOut);  //setting port8 pin 8 as output pin
  iolib_setdir(port9, bulbPin, DigitalOut);    //setting port9 pin 30 as output pin
    
  if(speed > threshold)
  {
   for(int i=0;i<2;i++)  //for turning off bulb and buzzer 8 times
   {
    for(int j=0;j<2;j++)  //for turn on and off buzzer and bulb for 2 times with delay
    {
     printf("BUZZER ON \n");
     pin_high(port8, buzzerPin);
     iolib_delay_ms(200);
     pin_low(port8, buzzerPin);
     iolib_delay_ms(200);

     printf("BULB FLASH \n");
     pin_low(port9, bulbPin);
     iolib_delay_ms(200);
     pin_high(port9, bulbPin);
     iolib_delay_ms(200);
     }
     iolib_delay_ms(900); //delay after two beeps and blinks
    iolib_delay_ms(900); //giving again delay since one iolib delay of 999ms
    }
     printf("BUZZER OFF\n");   //turning off both bulb and buzzer after 8 times
     pin_low(port8,buzzerPin);
     printf("BULB OFF\n");
     pin_high(port9,bulbPin);

   }


    else  //if over speed is not detected bulb and buzzer will be off
    {
    pin_low(port8, buzzerPin);                 //setting port8 pin low
    printf("BUZZER OFF \n");                   //print buzzer on
    pin_high(port9, bulbPin);                   //setting port9 pin low
    printf("BULB OFF \n");                     //print buzzer off
        }
                
  }  
  iolib_free();                               //freeing library
  return(0);  
}  
  


