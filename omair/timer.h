/*
 * timer.h
 *
 *  Created on: Oct 11, 2012
 *      Author: omair
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/time.h>                                   // Used for TimeOut operations
#include <stdlib.h>

/*!  \function     TimeOut
     \brief     This functions can manage a timer which is used as a timeout.
   */
extern void InitTimer();                                // Init the timer
extern unsigned long int   ElapsedTime_ms();            // Return the elapsed time since initialization
extern void delay_ms(int delay_time_ms);				// delay in ms by the time specidied by delay_time_ms


#endif /* TIMER_H_ */
