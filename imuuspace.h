#ifndef _IMUUSPACE_H
#define _IMUUSPACE_H

//#include "IMU.h"
#include "MinIMU9.h"
//#include "LSM303.h"
#include "itg3200.h"

class imuUspace : public MinIMU9
{
public:
    imuUspace(const char* i2cDeviceName);
    ITG3200 gyro;

};

//class imuUspace : public IMU {
//public:
//    imuUspace(const char * i2cDeviceName);

//    LSM303 compass;
//    ITG3200 gyro;

//    virtual vector readAcc();
//    virtual vector readMag();
//    virtual vector readGyro();

//    virtual void checkConnection();
//    virtual void enable();
//    virtual void loadCalibration();
//    virtual void measureOffsets();
//};

#endif
