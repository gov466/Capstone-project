#include <stdio.h> 							//Standard C input Output Library
#include <unistd.h> 							//defines miscellaneous symbolic constants and types, and declares miscellaneous functions
#include <string.h> 							//C Library for various String Operations
#include <termios.h> 							// Contains the definitions used by the terminal I/O interfaces 
#include <fcntl.h> 							// FIle control, Open, close
#include <sys/signal.h> 						//calls readback function whenever uart data comes
#include <time.h> 							//for providing delay function
#include <stdlib.h>  							//general purpose standard library which includes memmory allocation,conversions etc..
#include <sys/ioctl.h> 							//input/output cntrl //for reading and writing to uart
   

#define BAUDRATE B9600

int file, i; 								// Variable integers
char receive[20]; 							// declare a char array for receiving data
char buf[100]; 								// A buffer char array to store temporary data

int speed = 0;
int bytes_read = 0;
int bytes_written; 

//Function Declaration
void readback(int status);
void delay(unsigned int mseconds);
void setUART();

int main (void) 							// Main function
{
    

									// /dev/ttyO4 is linked with UART4 Port of Beaglebone BLE
    if ((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NONBLOCK))<0) 	// Try opening file in Read Write mode
    {	
        printf("UART4: Failed to open the file.\n"); 			//A message Print
        return 0;
    }    
    setUART();
	printf("\n Wait \n");
	
	while(1)
	{
		
		delay(1000);
		if(speed>0)
		{
			printf("Speed : %d\n",speed); 			//just printing value of speed
			
			
			speed = 0;
		}
		
		
	}
	
	
	printf("\n Done \n");
	
    close(file); 							//Close the file at last
}

void readback(int status)
{
	
	bytes_read = read(file,&receive,sizeof(receive)); 		// Read the incoming Message from arduino Module
									// Read the file and store the data in receive , read 100 bytes max
	
	
	if(bytes_read > 1) 						//If no. of bytes are read is more than 1
	{
									//printf("\n\nBytes Received - %d\n",bytes_read); 	
									// Print how many bytes was received
		printf("Got Speed: "); 					//print recieved data from bbb
		for(int i=0;i<bytes_read;i++) 				//a for loop to print data byte by byte
		{	
			if(receive[i] == 'z')				// 3z5
			{
					receive[i] = '4';  		// 345
			}
			
			printf("%c",receive[i]); 			//print a byte of message from Arduino Module
		}
		printf("\n###\n"); 					//General Print  
		
		bytes_written = write(file, receive, bytes_read); 	// Sending message to arduino Module
		
		sscanf(receive, "%d", &speed); 				//copying value of receive to variable speed
		
	}
	else
	{
		tcflush(file, TCIFLUSH);  				//for flushing any garbage data if there
	}
	tcflush(file, TCIFLUSH);
	
	
	
	
}



void delay(unsigned int mseconds) 					//delay in milliseconds
{
    clock_t goal = mseconds*1000 + clock(); 				//clock function came from time.h
    while (goal > clock());
}

void setUART()
{
	struct sigaction saio;           				/* definition of signal action */
    									/* install the signal handler before making the device asynchronous */
	saio.sa_handler = readback; 					//readback function called from here
									//saio.sa_mask = 0;
	sigemptyset(&saio.sa_mask);
	sigaddset(&saio.sa_mask, SIGINT);
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL); 					//sigio comes from <sys.signal.h> library
    fcntl(file, F_SETOWN, getpid());
    fcntl(file, F_SETFL, FASYNC);
 
	struct termios newtio;
									/* set new port settings for canonical input processing */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_cc[VMIN]=0;
	newtio.c_cc[VTIME]=1;
	tcflush(file, TCIFLUSH);
	tcsetattr(file,TCSANOW,&newtio);	
	tcflush(file, TCIOFLUSH);
	
}




