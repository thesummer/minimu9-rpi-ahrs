#include "itg3200.h"
#include <stdexcept>

#define ITG3200_ADDRESS_LOW     0b11010000
#define ITG3200_ADDRESS_HIGH    0b11010010

ITG3200::ITG3200(const char * i2cDeviceName) :
    i2c(i2cDeviceName)
{
    detectAddress();
}

void ITG3200::enable()
{
    writeReg(ITG3200_DLPF_FS, 0b00011000); // Range +-2000 deg/s 8kHz internal sample rate
    writeReg(ITG3200_SMPLRT_DIV, 80);      // Divide internal sample rate by 80 --> update rate 100 Hz
}

void ITG3200::writeReg(uint8_t reg, uint8_t value)
{
    i2c.writeByte(reg, value);
}

uint8_t ITG3200::readReg(uint8_t reg)
{
    return i2c.readByte(reg);
}

void ITG3200::read()
{
    uint8_t block[6];
    i2c.readBlock(ITG3200_GYRO_XOUT_H, sizeof(block), block);

    g[0] = (int16_t)(block[0] << 8 | block[1]);
    g[1] = (int16_t)(block[2] << 8 | block[3]);
    g[2] = (int16_t)(block[4] << 8 | block[5]);
}

void ITG3200::detectAddress()
{
    i2c.addressSet(ITG3200_ADDRESS_LOW);
    if (i2c.tryReadByte(ITG3200_WHO_AM_I) == ITG3200_ADDRESS_LOW) return;

    throw std::runtime_error("Reading WHO_AM_I-register failed.");
}

