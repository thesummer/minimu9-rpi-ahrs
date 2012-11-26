/*
 * usart.h
 *
 *  Created on: Oct 30, 2012
 *      Author: omair
 */

#ifndef USART_H_
#define USART_H_

extern unsigned char Receiver_Buffer[16];
extern int serial_init(void);
extern void *usart_read_routine();

#endif /* USART_H_ */
