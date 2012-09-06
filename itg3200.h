#ifndef ITG3200_H
#define ITG3200_H

#include "I2CBus.h"

#define ITG3200 WHO_AM_I            0x00
#define ITG3200_SMPLRT_DIV          0x15
#define ITG3200_DLPF_FS             0x16
#define ITG3200_INT_CFG             0x17
#define ITG3200_INT_STATUS          0x1A

#define ITG3200_TEMP_OUT_H          0x1B
#define ITG3200_TEMP_OUT_L          0x1C
#define ITG3200_GYRO_XOUT_H         0x1D
#define ITG3200_GYRO_XOUT_L         0x1E
#define ITG3200_GYRO_YOUT_H         0x1F
#define ITG3200_GYRO_YOUT_L         0x20
#define ITG3200_GYRO_ZOUT_H         0x21
#define ITG3200_GYRO_ZOUT_L         0x22

#define ITG3200_PWR_MGM             0x3E


class ITG3200
{
public:
    ITG3200(const char * i2cDeviceName);

    //gyro angular velocity readings
    int g[3];

    void enable(void);

    void writeReg(uint8_t reg, uint8_t value);

    uint8_t readReg(uint8_t reg);
    void read();

private:
    void detectAddress();
    I2CBus i2c;


};

#endif // ITG3200_H
