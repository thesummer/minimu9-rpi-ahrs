/*!
 \file      main.c

 \brief     main file for u-space communication protocol
            (USB to RS232 converter under linux).
            If the opening is successful, it sends the AT command
            and waits for a string being received from the device.
            After 5 seconds, if no valid data are received from the
            device, reception is giving up.

 \author    omair
 \version   1.0
 \date      24/09/2012
 */

#include <stdio.h>
#include <pthread.h>
#include "serialib.h"
#include "uspace_comm.h"
#include "crc.h"
#include "adc_7998.h"
#include "timer.h"
#include "usart.h"
#include "itpu.h"

//#define		ADC1_addr 	0x21
#define channel_num  8

const int ADC1_addr = 0x21 ;
const int ADC2_addr = 0x22 ;
int file_ADC1,file_ADC2;

int main(int argc, char *argv[])
{

	memory_alloc();
	serial_init();
    file_ADC1 = sensors_ADC_init(ADC1_addr);
//	file_ADC2 = sensors_ADC_init(ADC2_addr);


///////////////////////     Thread Creation    ////////////////////////////

		    pthread_t thread1;

           pthread_create( &thread1, NULL, usart_read_routine, NULL);


/////////////////// Set up ITPU ////////////////////////

           if (itpuInit(argc, argv) != 0)
           {
               printf("ERROR: Initializing ITPU failed\n");
               return 1;
           }

           itpuStart();

///////////////////  Write all the TMs   ///////////////           
		   while (1)
		   	   {
                   delay_ms(1000);

                   read_ADC(file_ADC1,ADC1_addr,channel_num);
//		   		   read_ADC(file_ADC2,ADC2_addr,channel_num);

		   		   TM_update();

		   		   printf ("Sending telemetries !\n");

		   		    send_TM(flag);
		   	   }


    return 0;
}


