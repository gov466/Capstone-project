#include <stdio.h> //Standard C input Output Library

#include <unistd.h> //defines miscellaneous symbolic constants and types, and declares miscellaneous functions

#include <string.h> //C Library for various String Operations

#include <termios.h> // Contains the definitions used by the terminal I/O interfaces

#include <fcntl.h> // FIle control, Open, close

#include <sys/signal.h>//calls readback function whenever uart data comes

#include <time.h> 			//for providing delay function

#include <stdlib.h>  			//general purpose standard library which includes memmory allocation,conversions etc..

#include <sys/ioctl.h> 			//input/output cntrl //for reading and writing to uart

#include <linux/i2c-dev.h>   //communicate with I2C devices

#include <iobb.h>   //input/output

#define I2C_BUS "/dev/i2c-2" // I2C bus device on a Beaglebone Black 
#define I2C_ADDR 0x27 // I2C slave address for the LCD module  

#define BAUDRATE B9600

int i2cFile;
int cursor_pos;

int file, file1, i; // Variable integers
char receive[20]; // declare a char array for receiving data
char buf[100]; // A buffer char array to store temporary data

int speed = 0;
int threshold = 60;

int bytes_read = 0;
int nbytes;
int bytes_written;

int relayPin = 30;
int buzzerPin = 8;

//Function Declaration
void readback(int status);
void delay(unsigned int mseconds);
void setUART();
void setUART1();
void send_simple_string_to_lcd(char * str);
void go_to(int line, char position);
void display_on();
void clear_display();
void i2c_send_byte(unsigned char data);
void i2c_start();
void lcd_init();

//https://github.com/xanthium-enterprises/Serial-Port-Programming-on-Linux/blob/master/USB2SERIAL_Read/Reciever%20(PC%20Side)/SerialPort_read.c

int main(void) // Main function
{

  iolib_init();
  iolib_setdir(9, relayPin, DigitalOut);
  iolib_setdir(8, buzzerPin, DigitalOut);
  pin_high(9, relayPin);
  pin_low(8, buzzerPin);

  i2c_start(); //Start I2C Communication

  lcd_init(); //Initialize the LCD

  clear_display(); //Clear the display

  // /dev/ttyO4 is linked with UART4 Port of Beaglebone 
  if ((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) // Try opening file in Read Write mode
  {
    printf("UART4: Failed to open the file.\n"); //A message Print
    return 0;
  }

  // /dev/ttyO4 is linked with UART4 Port of Beaglebone 
  if ((file1 = open("/dev/ttyO1", O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) // Try opening file in Read Write mode
  {
    printf("UART1: Failed to open the file.\n"); //A message Print
    return 0;
  }

  setUART();

  setUART1();

  printf("\n Wait \n");

  go_to(1, 0);
  send_simple_string_to_lcd("SPEED: --"); //Print a message on LCD

  while (1) {

    delay(1000);
    if (speed > 0) {
      printf("Speed : %d\n", speed);
      go_to(1, 0);
      send_simple_string_to_lcd("SPEED: "); //Print a message on LCD
      char str[5];
      sprintf(str, "%d", speed);
      send_simple_string_to_lcd(str);
      send_simple_string_to_lcd("km/h     ");

      if (speed > threshold) {
        go_to(2, 0);
        send_simple_string_to_lcd("   OVERSPEED   ");
        pin_low(9, relayPin); //Relay On 
        pin_high(8, buzzerPin);
        delay(1000);
        pin_high(9, relayPin); //Relay OFF
        pin_low(8, buzzerPin);

        printf("Sending data to ESP\n\n\n");
        memset(buf, 0, sizeof(buf));
        strcpy(buf, "1"); // Copy a string in buf char array
        nbytes = strlen(buf); // Store size of buf array in nbytes
        bytes_written = write(file1, buf, nbytes); // Sending message to ESP Module 

      } else {

        go_to(2, 0);
        send_simple_string_to_lcd("  NORMAL SPEED  ");
        pin_high(9, relayPin); //Relay OFF
        pin_low(8, buzzerPin);
      }
      speed = 0;
    }

  }

  printf("\n Done \n");

  close(file); //Close the file at last
}

void readback(int status) {

  bytes_read = read(file, & receive, sizeof(receive)); // Read the incoming Message from GSM Module
  // Read the file and store the data in receive , read 100 bytes max

  if (bytes_read > 1) //If no. of bytes are read is more than 1
  {
    //printf("\n\nBytes Received - %d\n",bytes_read); // Print how many bytes was received
    printf("Got Speed: ");
    for (int i = 0; i < bytes_read; i++) //a for loop to print data byte by byte
    {
      if (receive[i] == 'z') {
        receive[i] = '4';
      }

      printf("%c", receive[i]); //print a byte of message from GSM Module
    }
    printf("\n###\n"); //General Print  

    bytes_written = write(file, receive, bytes_read); // Sending message to GSM Module

    sscanf(receive, "%d", & speed);

  } else {
    tcflush(file, TCIFLUSH);
  }
  tcflush(file, TCIFLUSH);

}

void delay(unsigned int mseconds) {
  clock_t goal = mseconds * 1000 + clock();
  while (goal > clock());
}

void setUART() {
  struct sigaction saio; /* definition of signal action */
  /* install the signal handler before making the device asynchronous */
  saio.sa_handler = readback;
  //saio.sa_mask = 0;
  sigemptyset( & saio.sa_mask);
  sigaddset( & saio.sa_mask, SIGINT);
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO, & saio, NULL);
  fcntl(file, F_SETOWN, getpid());
  fcntl(file, F_SETFL, FASYNC);

  struct termios newtio;
  /* set new port settings for canonical input processing */
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;
  newtio.c_cc[VMIN] = 0;
  newtio.c_cc[VTIME] = 1;
  tcflush(file, TCIFLUSH);
  tcsetattr(file, TCSANOW, & newtio);
  tcflush(file, TCIOFLUSH);

}

void setUART1() {
  // struct sigaction saio1;           /* definition of signal action */
  //    /* install the signal handler before making the device asynchronous */
  // saio1.sa_handler = readback;
  // //saio.sa_mask = 0;
  // sigemptyset(&saio1.sa_mask);
  // sigaddset(&saio1.sa_mask, SIGINT);
  // saio1.sa_flags = 0;
  // saio1.sa_restorer = NULL;
  // sigaction(SIGIO,&saio1,NULL);
  //    fcntl(file1, F_SETOWN, getpid());
  //    fcntl(file1, F_SETFL, FASYNC);

  struct termios newtio1;
  /* set new port settings for canonical input processing */
  newtio1.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio1.c_iflag = IGNPAR | ICRNL;
  newtio1.c_oflag = 0;
  newtio1.c_lflag = ICANON;
  newtio1.c_cc[VMIN] = 0;
  newtio1.c_cc[VTIME] = 1;
  tcflush(file1, TCIFLUSH);
  tcsetattr(file1, TCSANOW, & newtio1);
  tcflush(file1, TCIOFLUSH);

}

void i2c_start() //Function to start i2c communication
{
  if ((i2cFile = open(I2C_BUS, O_RDWR)) < 0) {
    printf("Error failed to open I2C bus [%s].\n", I2C_BUS);
    exit(-1);
  }
  // set the I2C slave address for all subsequent I2C device transfers  
  if (ioctl(i2cFile, I2C_SLAVE, I2C_ADDR) < 0) {
    printf("Error failed to set I2C address [%s].\n", I2C_ADDR);
    exit(-1);
  }
}

void i2c_send_byte(unsigned char data) //Sens a byte of data via i2c 
{
  unsigned char byte[1];
  byte[0] = data;
  write(i2cFile, byte, sizeof(byte));
  /* -------------------------------------------------------------------- * 
   * Below wait creates 1msec delay, needed by display to catch commands  * 
   * -------------------------------------------------------------------- */
  usleep(1000);
}

void clear_display() //Function to clear Display
{
  /* -------------------------------------------------------------------- * 
   * Display clear, cursor home                                           * 
   * -------------------------------------------------------------------- */
  usleep(40); // wait 40usec  
  i2c_send_byte(0b00000100); //  
  i2c_send_byte(0b00000000); // D7-D4=0  
  i2c_send_byte(0b00010100); //  
  i2c_send_byte(0b00010000); // D0=display_clear  

}

void display_on() {
  /* -------------------------------------------------------------------- * 
   * Turn on the display                                                  * 
   * -------------------------------------------------------------------- */
  usleep(40); // wait 40usec  
  i2c_send_byte(0b00001100); //  
  i2c_send_byte(0b00001000); // D7-D4=0  
  i2c_send_byte(0b11101100); //  
  i2c_send_byte(0b11101000); // D3=1 D2=display_on, D1=cursor_on, D0=cursor_blink  
  cursor_pos = 1;
}

void go_to(int line, char position) //Function to move cursor of lcd
{
  if (line == 1) position = 0x80 + position;

  else if (line == 2) position = 0xC0 + position;

  int i;

  char n1, n2, ln1, ln2, mask1, mask2;

  ln1 = 0b11111100;
  ln2 = 0b11111000;

  mask1 = 0b11110000;
  mask2 = 0b00001111;

  n1 = (position & mask1) | mask2;
  n2 = ((position & mask2) << 4) | mask2;

  i2c_send_byte(n1 & ln1);
  i2c_send_byte(n1 & ln2);
  i2c_send_byte(n2 & ln1);
  i2c_send_byte(n2 & ln2);

}

void send_simple_string_to_lcd(char * str) //A function to send Simple String to LCD 
{

  int i;
  for (i = 0;
    (str[i] != 0x00); i++) {

    char chr = str[i];

    int i;

    char n1, n2, ln1, ln2, mask1, mask2;

    ln1 = 0b11111101;
    ln2 = 0b11111001;

    mask1 = 0b11110000;
    mask2 = 0b00001111;

    n1 = (chr & mask1) | mask2;
    n2 = ((chr & mask2) << 4) | mask2;

    i2c_send_byte(n1 & ln1);
    i2c_send_byte(n1 & ln2);
    i2c_send_byte(n2 & ln1);
    i2c_send_byte(n2 & ln2);

  }

}

void lcd_init() {

  /* -------------------------------------------------------------------- * 
   * Initialize the display, using the 4-bit mode initialization sequence * 
   * -------------------------------------------------------------------- */

  sleep(0.4); // wait 40msec  
  i2c_send_byte(0b00110100); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=1  
  i2c_send_byte(0b00110000); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=0  

  sleep(0.1); // wait 10msec  
  i2c_send_byte(0b00110100); //   
  i2c_send_byte(0b00110000); // same  
  sleep(0.1); // wait 10msec  
  i2c_send_byte(0b00110100); //  
  i2c_send_byte(0b00110000); // 8-bit mode init complete  
  sleep(0.1); // wait 10msec  
  i2c_send_byte(0b00100100); //  
  i2c_send_byte(0b00100000); // switched now to 4-bit mode  

  /* -------------------------------------------------------------------- * 
   * 4-bit mode initialization complete. Now configuring the function set * 
   * -------------------------------------------------------------------- */
  usleep(100); // wait 100usec  
  i2c_send_byte(0b00100100); //  
  i2c_send_byte(0b00100000); // keep 4-bit mode  
  i2c_send_byte(0b10000100); //  D3=2lines
  i2c_send_byte(0b10000000); // D2=char5x8  

  clear_display();

  display_on();

  sleep(1); //delay of 1 second	
}
