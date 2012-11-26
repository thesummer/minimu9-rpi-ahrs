/*
 * timer.c
 *
 *  Created on: Oct 11, 2012
 *      Author: omair
 */



#include "timer.h"

 struct timeval      PreviousTime;

//Initialize the timer
void InitTimer()
{
    gettimeofday(&PreviousTime, NULL);
}

/*!
    \brief      Returns the time elapsed since initialization.  It write the current time of the day in the structure CurrentTime.
                Then it returns the difference between CurrentTime and PreviousTime.
    \return     The number of microseconds elapsed since the functions InitTimer was called.
  */
//Return the elapsed time since initialization
unsigned long int ElapsedTime_ms()
{
    struct timeval CurrentTime;
    int sec,usec;
    gettimeofday(&CurrentTime, NULL);                                   // Get current time
    sec=CurrentTime.tv_sec-PreviousTime.tv_sec;                         // Compute the number of second elapsed since last call
    usec=CurrentTime.tv_usec-PreviousTime.tv_usec;			// Compute
    if (usec<0) {														// If the previous usec is higher than the current one
        usec=1000000-PreviousTime.tv_usec+CurrentTime.tv_usec;		// Recompute the microseonds
        sec--;														// Substract one second
    }
    return sec*1000+usec/1000;
}

void delay_ms(int delay_time_ms)

{
	unsigned long int Time_out=0;

	InitTimer();

	while (Time_out<delay_time_ms)
			{
				Time_out = ElapsedTime_ms();
			}
}

