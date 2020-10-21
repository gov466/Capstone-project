#include <Wire.h>  //library used to communicate with i2c devices
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup()
{
  //  LCD with I2C
  // initialize the LCD, 
  lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
}
void loop()
{
  //start of loop  for LCD with I2C
  lcd.clear();
  lcd.print("Car Over-Speed ");
  lcd.setCursor (0,1); // go to start of 2nd line
 lcd.print("Detection System");
   
  delay(500);
 //end of loopcode for LCD with I2C
}
