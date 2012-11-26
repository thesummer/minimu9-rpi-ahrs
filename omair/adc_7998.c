#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "timer.h"
#include "adc_7998.h"
#include "uspace_comm.h"


// Address pointer Register


unsigned char ADDR_CONF_REG = 0x02;
unsigned char AD7998_CONFIG_REG [2]= {0x0F,0xF8};		   // reading all 8 channels with filtering active
unsigned char ADDR_CONV_RES_REG = 0x00;
char command = 0x70;				// command for mode 2

unsigned char tem_buffer[3]={0};


int sensors_ADC_init(int addr)
{
	int file;
    char filename[40];

    sprintf(filename,"/dev/i2c-2");
    if ((file = open(filename,O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        exit(1);
    }
    if (ioctl(file,I2C_SLAVE,addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        exit(1);
           }

    tem_buffer[0] = ADDR_CONF_REG;
    tem_buffer[1] = AD7998_CONFIG_REG [0];
    tem_buffer[2] = AD7998_CONFIG_REG [1];

    if (write(file,tem_buffer,3) != 3) {
       printf("Failed to write CONFIG register \n");
        }else
    	{	printf("Contents of CONFIG REG are:");
			read_reg(file,2);
    	}

    return file;

    }


int read_ADC(int file, int ADC_addr , int num_channel)
{

	char buf[16] = {0};
	float data;
	char channel;
	int i,ret;
	struct i2c_rdwr_ioctl_data i2c_data;
	struct i2c_msg msg[2];


	     i2c_data.msgs = msg;
	     i2c_data.nmsgs = 2;     // two i2c_msg


	     i2c_data.msgs[0].addr = ADC_addr;
	     i2c_data.msgs[0].flags = 0;     // write
	     i2c_data.msgs[0].len = 1;       // only one byte
	     i2c_data.msgs[0].buf = &command;

		 /*
		 * Second, read data from the ADC
		 */
		 i2c_data.msgs[1].addr = ADC_addr;
		 i2c_data.msgs[1].flags = I2C_M_RD;      // read command
		 i2c_data.msgs[1].len = num_channel*2;
		 i2c_data.msgs[1].buf = buf;

		 ret = ioctl(file, I2C_RDWR, &i2c_data);

		        if (ret < 0) {
		                    perror("read data fail\n");
		                    return ret;
		            }else
		            	{	
				 printf("ADC %X\n",ADC_addr);
	
		            		for(i = 0; i<num_channel*2; i=i+2)

		            			{
		            				data = (float)((buf[i] & 0b00001111)<<8)+buf[i+1];
		            				data = (data/4096)*5;
		            				channel = ((buf[i] & 0b01110000)>>4);

		            				switch (ADC_addr)
		            					{
		            						case 0x21:
		            							ADC1_update(channel,data);
		            							break;
		            						case 0x22:
		            							ADC2_update(channel,data);
		            							break;
		            						default:
		            							printf("no match of ADC_ADDRESS");
		            							break;
		            					}

		            				printf("Channel %03d Data:  %04f\n",channel,data);
		            			}
		            	}
	    return 1;
}

int ADC1_update(char channel_ID, float update_data)
{
	switch (channel_ID)
	{
		case 0:
			power->V_cell_1 = update_data; break;
		case 1:
			power->N_C = update_data; break;
		case 2:
			power->V_cell_2 = update_data; break;
		case 3:
			power->I_charge = update_data; break;
		case 4:
			power->V_uvlo = update_data; break;
		case 5:
			power->charge_status = update_data; break;
		case 6:
			power->I_discharge = update_data; break;
		case 7:
			power->T_BAT = update_data; break;
		default:
			printf("no match of channel_ID"); break;
	}
	return 0;
}


int ADC2_update(char channel_ID, float update_data)
{
	switch (channel_ID)
	{
		case 0:
			OBDH->temp_4 = update_data; break;
		case 1:
			OBDH->temp_6 = update_data; break;
		case 2:
			OBDH->temp_2 = update_data; break;
		case 3:
			OBDH->temp_8 = update_data; break;
		case 4:
			power->I_3_3v = update_data; break;
		case 5:
			power->V_5_0v = update_data; break;
		case 6:
			power->I_5_0v = update_data; break;
		case 7:
			power->V_3_3v = update_data; break;
		default:
			printf("no match of channel_ID"); break;
	}
	return 0;
}



int read_reg(int file, int size)
{
char buf_reg[3] = {0};
if (read(file,buf_reg,size) != size) {
                    /* ERROR HANDLING: i2c transaction failed */
                    printf("Failed to read from the i2c bus.\n");

                        } else
				 {
					if (size==1){
                                        		printf(":%X\n",buf_reg[0]);
                                           	    }	else
								 {
									 printf(": %X and %X \n ",buf_reg[0],buf_reg[1]);
								 }
				}

return 0;
}
