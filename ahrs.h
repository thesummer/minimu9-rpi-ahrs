#ifndef AHRS_H
#define AHRS_H

#include<shareddata.h>
#include<imuuspace.h>
#include<stdlib.h>

class ahrs;

typedef void (ahrs::*fuse_function) (quaternion& rotation, float dt, const vector& angular_velocity,
                  const vector& acceleration, const vector& magnetic_field);

class ahrs
{
public:
    ahrs(sharedData& sD, std::string i2cDev);
    enum fuseType {normal, raw, compass_only, gyro_only};
    void setFuseType(fuseType newMode);
    fuseType getFuseType() { return mMode;}
    bool start();
    bool stop();

private:
    sharedData& mSharedData; //reference
    std::string mI2CDevice;
    imuUspace mImu;
    fuse_function mFuse;
    pthread_t mThread;
    fuseType mMode;
    bool mRunning;

    static void*  callThreadFunction(void *arg) { return ((ahrs*)arg)->threadAhrs(); }

    void * threadAhrs();

    matrix rotationFromCompass(const vector& acceleration, const vector& magnetic_field);
    void rotate(quaternion& rotation, const vector& w, float dt);

    void fuse_default(quaternion& rotation, float dt, const vector& angular_velocity,
      const vector& acceleration, const vector& magnetic_field);

    void fuse_compass_only(quaternion& rotation, float dt, const vector& angular_velocity,
      const vector& acceleration, const vector& magnetic_field);

    void fuse_gyro_only(quaternion& rotation, float dt, const vector& angular_velocity,
      const vector& acceleration, const vector& magnetic_field);

    void fuse_raw(quaternion& rotation, float dt, const vector& angular_velocity,
      const vector& acceleration, const vector& magnetic_field);

};


#endif // AHRS_H
