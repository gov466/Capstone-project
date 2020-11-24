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
   for(int i=0;i<8;i++)
   {
    for(int j=0;j<2;j++)
    {
     printf("BUZZER ON \n");
     pin_high(port8, buzzerPin);
     iolib_delay_ms(300);
     pin_low(port8, buzzerPin);
     iolib_delay_ms(300);

     printf("BULB FLASH \n");
     pin_low(port9, bulbPin);
     iolib_delay_ms(300);
     pin_high(port9, bulbPin);
     iolib_delay_ms(300);
     }
     iolib_delay_ms(900);
    iolib_delay_ms(600);
    }
     printf("BUZZER OFF\n");
     pin_low(port8,buzzerPin);
     printf("BULB OFF\n");
     pin_high(port9,bulbPin);

   }


    else
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
  


