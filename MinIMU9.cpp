#include "vector.h"
#include "MinIMU9.h"
#include "exceptions.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <wordexp.h>

MinIMU9::MinIMU9(const char * i2cDeviceName) :
  compass(i2cDeviceName), gyro(i2cDeviceName)
{
}

void MinIMU9::enable()
{
    compass.enable();
    gyro.enable();
}

void MinIMU9::loadCalibration()
{
    wordexp_t expansion_result;
    wordexp("~/.minimu9-ahrs-cal", &expansion_result, 0);

    std::ifstream file(expansion_result.we_wordv[0]);
    if (file.fail())
    {
        throw posix_error("Failed to open calibration file ~/.minimu9-ahrs-cal.");
    }
    
    file >> calMatrix(0,0) >> calMatrix(0,1) >> calMatrix(0,2);
    file >> calMatrix(1,0) >> calMatrix(1,1) >> calMatrix(1,2);
    file >> calMatrix(2,0) >> calMatrix(2,1) >> calMatrix(2,2);

    file >> calOffset(0) >> calOffset(1) >> calOffset(2);

    if (file.fail() || file.bad())
    {
        throw std::runtime_error("Failed to parse calibration file ~/.minimu9-ahrs-cal.");
    }
    
}

void MinIMU9::measureOffsets()
{
    // LSM303 accelerometer: 8 g sensitivity.  3.8 mg/digit; 1 g = 256.
    // TODO: unify this with the other place in the code where we scale accelerometer readings.
    gyro_offset = vector::Zero();
    const int sampleCount = 32;
    for(int i = 0; i < sampleCount; i++)
    {
        gyro.read();
        gyro_offset += vector_from_ints(&gyro.g);
        usleep(20*1000);
    }
    gyro_offset /= sampleCount;
}

vector MinIMU9::readMag()
{
    compass.readMag();
    IMU::raw_m = int_vector_from_ints(&compass.m);
    
    vector v;
    v = IMU::raw_m.cast<float>() - calOffset;
    v = calMatrix * v;
//    v(0) = (float)(compass.m[0] - mag_min(0)) / (mag_max(0) - mag_min(0)) * 2 - 1;
//    v(1) = (float)(compass.m[1] - mag_min(1)) / (mag_max(1) - mag_min(1)) * 2 - 1;
//    v(2) = (float)(compass.m[2] - mag_min(2)) / (mag_max(2) - mag_min(2)) * 2 - 1;
    return v;
}

vector MinIMU9::readAcc()
{
    // LSM303 accelerometer: At 8 g sensitivity, the datasheet says
    // we get 3.9 mg/digit.
    // TODO: double check this figure using the correct datasheet
    const float accel_scale = 0.0039;

    compass.readAcc();
    IMU::raw_a = int_vector_from_ints(&compass.a);
    return vector_from_ints(&compass.a) * accel_scale;
}

vector MinIMU9::readGyro()
{
    // At the full-scale=2000 dps setting, the gyro datasheet says
    // we get 0.07 dps/digit.
    const float gyro_scale = 0.07 * 3.14159265 / 180;

    gyro.read();
    IMU::raw_g = int_vector_from_ints(&gyro.g);
    return ( vector_from_ints(&gyro.g) - gyro_offset ) * gyro_scale;
}
