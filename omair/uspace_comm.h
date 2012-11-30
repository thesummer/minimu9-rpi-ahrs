/*
 * uspace_comm.h
 *
 *  Created on: Jun 28, 2012
 *      Author: omair
 */

#ifndef USPACE_H_
#define USPACE_H_

#include "serialib.h"
#include "crc.h"
#include <stdio.h>

 typedef struct{

	    float V_uvlo;
	    float V_cell_2;
	    float I_discharge;
	    float V_cell_1;
	    float T_BAT;
	    float N_C;
	    float charge_status;
	    float I_charge;
	    float I_3_3v;
	    float I_5_0v;
	    float V_3_3v;
	    float V_5_0v;
	}power_subsystem;

typedef struct {

	float AHRS[3];


	} ADCS_subsystem;

typedef struct {

	float temp_2;
	float temp_4;
	float temp_6;
	float temp_8;
	float UTC_time[3];

} OBDH_subsystem;


typedef struct {

	float latitude;
	float longitude;
	float altitude;
	float UTC_time;

} GPS_subsystem;

extern unsigned char flag;

extern power_subsystem *power;
extern ADCS_subsystem *ADCS;
extern OBDH_subsystem *OBDH;
extern GPS_subsystem *GPS;

// TM function prototypes
extern void memory_alloc ();
extern int TM_update();
extern int TC_decoder();
extern int tm_write(int TM_ID, float TM_values[], int num_of_TM);
extern void tm_power();
extern void tm_OBDH();
extern void tm_GPS();
extern void tm_ADCS();
extern int serial_init(void);
int send_TM(unsigned char flag_set);
int GPS_decoder();
int ADCS_decoder();
int OBDH_decoder();
int power_decoder();


#endif /* USPACE_H_ */


