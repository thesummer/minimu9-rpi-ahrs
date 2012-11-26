/*
 * adc_7998.h
 *
 *  Created on: Sep 29, 2012
 *      Author: omair
 */

#ifndef ADC_7998_H_
#define ADC_7998_H_


extern int sensors_ADC_init(int addr);
extern int read_ADC(int file, int ADC_addr, int num_channel);
extern int read_reg(int file, int size);
extern int ADC1_update(char channel_ID, float update_data);
extern int ADC2_update(char channel_ID, float update_data);

#endif /* ADC_7998_H_ */
