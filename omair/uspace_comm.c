/*
 * uspace_comm.c
 *
 *  Created on: Jun 28, 2012
 *      Author: omair
 */

#include <stdio.h>
#include "uspace_comm.h"
#include "serialib.h"
#include "adc_7998.h"
#include "crc.h"
#include "timer.h"
#include "usart.h"
#include "itpu.h"
#include "gpsData.h"

// sync byte to synchronize the ground station and U-SPACE

unsigned char uspace_sync = 0x0C;
unsigned char flag = 0;

// TM_ID of power subsystem  //  subsystem has an ID of "0x11"

// ADC 1 connector

unsigned int V_uvlo_ID   		=  0x1141;
unsigned int V_cell_2_ID     	=  0x1142;
unsigned int I_discharge_ID   	=  0x1143;
unsigned int V_cell_1_ID     	=  0x1144;
unsigned int T_BAT_ID        	=  0x1145;
unsigned int N_C_ID   	        =  0x1146;
unsigned int charge_status_ID 	=  0x1147;
unsigned int I_charge_ID      	=  0x1148;

// ADC 2 connector

unsigned int I_3_3v_ID   		=  0x1149;
unsigned int I_5_0v_ID   		=  0x114A;
unsigned int V_3_3v_ID     		=  0x114B;
unsigned int V_5_0v_ID     		=  0x114C;


// TM_ID of OBDH subsystem // subsystem has an ID of "0x33"

// ADC 2 connector (remaining 4 TMs)

unsigned int temp_2_ID 	  =  0x3351;
unsigned int temp_4_ID	  =  0x3352;
unsigned int temp_6_ID    =  0x3353;
unsigned int temp_8_ID    =  0x3354;


// TM_ID of ADCS subsystem // subsystem has an ID of "0x55"

unsigned int AHRS_ID = 0x5561;


// TM_ID of GPS subsystem // subsystem has an ID of "0x77"

unsigned int latitude_ID 	   =  0x7771;
unsigned int longitude_ID      =  0x7772;
unsigned int altitude_ID  	   =  0x7773;
unsigned int UTC_time_ID       =  0x7774;


/* telemeteries for u-space categorized according to subsystems*/

power_subsystem *power;

ADCS_subsystem *ADCS;

GPS_subsystem *GPS;

OBDH_subsystem *OBDH;


/* writes telemetry to the USART. Following inputs are required:
   * header: header code for telemetry unpacking on ground
   * values: array of values to be sent
   */


 int tm_write(int TM_ID, float TM_values[], int num_of_TM)
  {
  	unsigned char tm_buffer[40]={0};
  	unsigned char TM_ID_HIGH,TM_ID_LOW , CRC , size;
  	unsigned int i,Ret;

		TM_ID_LOW  = (unsigned char)(TM_ID & 0xFF);
        TM_ID_HIGH = (unsigned char)((TM_ID>>8) & 0xFF);
  	 	// Write the header for TM
  	sprintf(tm_buffer,"%c%c%c%i", uspace_sync, TM_ID_HIGH, TM_ID_LOW, num_of_TM);

  	for (i=0; i < num_of_TM; i++)
  	{
  		// Write the TM one at a time
  		sprintf(tm_buffer, "%s%10.4f",tm_buffer,TM_values[i]);

  	}

  	// determine CRC byte
  	if (num_of_TM == 1)
  	{
  		size = 14;
  		tm_buffer[15]='\0';
  	}
  	else
  	{
  		size = 34;
  		tm_buffer[35]='\0';
  	}

  	CRC = updateCRC(0, tm_buffer, size);

  	// Write the CRC byte for TM
  	sprintf(tm_buffer, "%s%c", tm_buffer, CRC);

  	// Write the tm_buffer on the serial port

  	    Ret= WriteString(tm_buffer);                                             // Send the command on the serial port
  	    if (Ret!=1) {                                                           // If the writting operation failed ...
  	        printf ("Error while writing data\n");                              // ... display a message ...
  	        return Ret;                                                         // ... quit the application.
  	    }
  	    printf ("Write operation is successful : %d \n",strlen(tm_buffer));
  	  return Ret;
  }


  // Write power telemetries
    void tm_power()
    {

	  tm_write (V_uvlo_ID,&(power->V_uvlo),1);
	  tm_write (V_cell_2_ID,&(power->V_cell_2),1);
	  tm_write (I_discharge_ID,&(power->I_discharge),1);
	  tm_write (V_cell_1_ID,&(power->V_cell_1),1);

	  tm_write (T_BAT_ID,&(power->T_BAT),1);
	  tm_write (N_C_ID,&(power->N_C),1);
	  tm_write (charge_status_ID,&(power->charge_status),1);
	  tm_write (I_charge_ID,&(power->I_charge),1);

	  tm_write (I_3_3v_ID,&(power->I_3_3v),1);
	  tm_write (I_5_0v_ID,&(power->I_5_0v),1);
	  tm_write (V_3_3v_ID,&(power->V_3_3v),1);
	  tm_write (V_5_0v_ID,&(power->V_5_0v),1);
  }

  // Write OBDH telemetries
  void tm_OBDH()
    {

	  tm_write (temp_2_ID,&(OBDH->temp_2),1);
	  tm_write (temp_4_ID,&(OBDH->temp_4),1);
	  tm_write (temp_6_ID,&(OBDH->temp_6),1);
	  tm_write (temp_8_ID,&(OBDH->temp_8),1);

    }

  // Write GPS telemetries

  void tm_GPS()
    {

	  tm_write (latitude_ID,&(GPS->latitude),1);
	  tm_write (longitude_ID,&(GPS->longitude),1);
	  tm_write (altitude_ID,&(GPS->altitude),1);
	  tm_write (UTC_time_ID,&(GPS->UTC_time),1);

    }

// Write ADCS telemetries

  void tm_ADCS()
    {
	  tm_write (AHRS_ID,ADCS->AHRS,3);


    }
  void memory_alloc()
  {
		power = (power_subsystem*) malloc( sizeof(power_subsystem) );
		ADCS  = (ADCS_subsystem*) malloc( sizeof(ADCS_subsystem) );
		OBDH  = (OBDH_subsystem*) malloc( sizeof(OBDH_subsystem) );
		GPS   = (GPS_subsystem*) malloc( sizeof(GPS_subsystem) );
  }


int TM_update()
{
    float euler[3];
    getEulerAngle(euler);

    struct gpsData gps = getGps();


    GPS->latitude = gps.latitude;
    GPS->longitude = gps.longitude;
    GPS->altitude = gps.height;
    GPS->UTC_time = 10.10;   //format??

    ADCS->AHRS[0] = euler[0];
    ADCS->AHRS[1] = euler[1];
    ADCS->AHRS[2] = euler[2];

return 0;
}

int TC_decoder()
{
	int result;
	if (Receiver_Buffer[0]	!=	 uspace_sync)
		{
			return 0;
		}

	switch(Receiver_Buffer[1])
		{
			case 0x11:
				result = power_decoder();
				printf("match of power Subsystem_ID\n");
				break;
			case 0x33:
				printf("match of OBDH Subsystem_ID\n");
				result = OBDH_decoder();
				break;
			case 0x55:
				result = ADCS_decoder();
				printf("match of ADCS Subsystem_ID\n");
				break;
			case 0x77:
				result = GPS_decoder();
				printf("match of GPS Subsystem_ID\n");
				break;
			default:
				result = 0;
				printf("No match of Subsystem_ID\n");
				break;
		}
return result;
}

int power_decoder()
{
	int result;

	switch (Receiver_Buffer[2])
		{
			case 0x11:
				result = 1;
				break;
  			case 0x33:
  				result = 1;
  				break;
  			case 0x55:
  				result = 1;
  				break;
  			case 0x77:
  				result = 1;
  				break;
  			default:
  				result = 0;
  				printf("No match of Power Subsystem_ID\n");
  				break;
		}
	return result;
}

int OBDH_decoder()
{
	int result;

	switch (Receiver_Buffer[2])
		{
			case 0xB1:						// set flag for telemetry
				flag = Receiver_Buffer[13];
				printf("TM flag was set\n");
				result = 1;
				break;
  			case 0xB2:
  				result = 1;
  				break;
  			case 0xB3:
  				result = 1;
  				break;
  			case 0xB4:
  				result = 1;
  				break;
  			default:
  				result = 0;
  				printf("No match of OBDH Subsystem_ID\n");
  				break;
		}
	return result;
}

int ADCS_decoder()
{
	int result;

	switch (Receiver_Buffer[2])
		{
			case 0xA1:
				result = 1;
				break;
  			case 0xA2:
  				result = 1;
  				break;
  			case 0xA3:
  				result = 1;
  				break;
  			default:
  				result = 0;
  				printf("No match of ADCS Subsystem_ID\n");
  				break;
		}
	return result;
}

int GPS_decoder()
{
	int result;

	switch (Receiver_Buffer[2])
		{
			case 0x11:
				result = 1;
				break;
  			case 0x33:
  				result = 1;
  				break;
  			case 0x55:
  				result = 1;
  				break;
  			case 0x77:
  				result = 1;
  				break;
  			default:
  				result = 0;
  				printf("No match of GPS Subsystem_ID");
  				break;
		}
	return result;
}

int send_TM(unsigned char flag_set)
{
	int result;

		switch (flag)
			{
				case 0x00:            // donot send telemetry

					result = 1;
					break;

				case 0x10:			  // send only power telemetry

					tm_power();
	  				result = 1;
	  				break;

				case 0x20:			  // send only OBDH telemetry

					tm_OBDH();
	  				result = 1;
	  				break;

				case 0x40:            // send only ADCS telemetry

					tm_ADCS();
	  				result = 1;
	  				break;

				case 0x50:            // send only GPS telemetry

					tm_GPS();
	  				result = 1;
	  				break;

				case 0x70:            // send all telemetry

					tm_power();
	  				tm_OBDH();
	  				tm_ADCS();
	  				tm_GPS();
	  				result = 1;
	   				break;

				default:

					result = 0;
	  				printf("No match of TM setup");
	  				break;
			}
return result;

}
