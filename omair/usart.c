/*
 * usart.c
 *
 *  Created on: Oct 30, 2012
 *      Author: omair
 */

#include "usart.h"
#include <pthread.h>
#include "serialib.h"
#include "uspace_comm.h"


pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
#define DEVICE_PORT 		"/dev/ttyO2"                         // ttyO2 for beaglebord
#define DEVICE_SPEED 		115200								// data rate
unsigned char Receiver_Buffer[16];
// Open serial port

int serial_init(void)
{
	int Ret;
    Ret=Open(DEVICE_PORT,DEVICE_SPEED);                                     // Open serial link at 115200 bauds
    if (Ret!=1) {                                                           // If an error occured...
        printf ("Error while opening port. Permission problem ?\n");        // ... display a message ...
        return Ret;                                                         // ... quit the application
    }
    printf ("Serial port opened successfully !\n");
    return Ret;
}

/*
 * Routine for USART thread for getting telecommands
 *
 * */
    void *usart_read_routine(void * params)
	{

	int Ret1;


	      // Lock mutex
	      pthread_mutex_lock( &count_mutex );

	      while (1)
	      	  {
	    	  	  // Read a string from the serial device
                  Ret1=ReadString((char*) Receiver_Buffer,'\r',16,5000);                           // Read a maximum of 128 characters with a timeout of 5 seconds
	                                                                               // The final character of the string must be a line feed ('\r')
	    	  	  if (Ret1>0)                                                      // If a string has been read from, print the string
	    	  		  {
	    	  		  	   printf ("String read from serial port : %s\n",Receiver_Buffer);
	    	  		  	   TC_decoder();
	    	  		  }
	    	  	  else
	    	  		  printf ("TimeOut reached. No data received !\n");            // If not, print a message.
	      	  }

	      pthread_mutex_unlock( &count_mutex );
	      return(NULL);
	    }


