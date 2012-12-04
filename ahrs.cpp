#include "vector.h"
#include "imuuspace.h"

#include <system_error>
#include <thread.h>
#include <pthread.h>

#include<ahrs.h>


ahrs::ahrs(sharedData& sD, std::string i2cDev):
    mSharedData(sD), mI2CDevice(i2cDev), mImu(i2cDev.c_str()),
    mRunning(false)
{
    setFuseType(normal);
}

void ahrs::setFuseType(fuseType newMode)
{
    mMode = newMode;
    switch (newMode)
    {
    default: mMode = normal;
    case normal         : mFuse = &ahrs::fuse_default;      break;
    case raw            : mFuse = &ahrs::fuse_raw;          break;
    case compass_only   : mFuse = &ahrs::fuse_compass_only; break;
    case gyro_only      : mFuse = &ahrs::fuse_gyro_only;    break;
    }
}

bool ahrs::start()
{
    if (mRunning)
    {
        return false;
    }

    if (mMode != raw)
    {
        mImu.loadCalibration();
    }
    mImu.enable();
    mImu.measureOffsets();

    pthread_create (&mThread, NULL, ahrs::callThreadFunction, this) ;
    mRunning = true;

    return true;
}

bool ahrs::stop()
{
    if(!mRunning)
    {
        return false;
    }

    pthread_cancel(mThread);
    pthread_join(mThread, NULL);
    mRunning = false;

    return true;
}

void * ahrs::threadAhrs()
{
    struct periodic_info info;
    makePeriodic(20000, &info);
    float dt = 20000/1000000.0;

    quaternion rotation = quaternion::Identity();

    while(1)
    {
        vector angular_velocity = mImu.readGyro();
        vector acceleration = mImu.readAcc();
        vector magnetic_field = mImu.readMag();

        (this->*mFuse) (rotation, dt, angular_velocity, acceleration, magnetic_field);
        mSharedData.setRotation(rotation);
//        std::cout << mSharedData.getRotation().toRotationMatrix().eulerAngles(2, 1, 0) * (180 / M_PI);
        waitPeriod(&info);
    }
    return (void*) NULL;
}


//! Uses the acceleration and magnetic field readings from the compass
// to get a noisy estimate of the current rotation matrix.
// This function is where we define the coordinate system we are using
// for the ground coords:  North, East, Down.
matrix ahrs::rotationFromCompass(const vector& acceleration, const vector& magnetic_field)
{
    vector down = -acceleration;     // usually true
    vector east = down.cross(magnetic_field); // actually it's magnetic east
    vector north = east.cross(down);

    east.normalize();
    north.normalize();
    down.normalize();

    matrix r;
    r.row(0) = north;
    r.row(1) = east;
    r.row(2) = down;
    return r;
}

//typedef void fuse_function(quaternion& rotation, float dt, const vector& angular_velocity,
//                  const vector& acceleration, const vector& magnetic_field);

void ahrs::fuse_compass_only(quaternion& rotation, float dt, const vector& angular_velocity,
  const vector& acceleration, const vector& magnetic_field)
{
    // Implicit conversion of rotation matrix to quaternion.
    rotation = rotationFromCompass(acceleration, magnetic_field);
}

// Uses the given angular velocity and time interval to calculate
// a rotation and applies that rotation to the given quaternion.
// w is angular velocity in radians per second.
// dt is the time.
void ahrs::rotate(quaternion& rotation, const vector& w, float dt)
{
    // Multiply by first order approximation of the
    // quaternion representing this rotation.
    rotation *= quaternion(1, w(0)*dt/2, w(1)*dt/2, w(2)*dt/2);
    rotation.normalize();
}

void ahrs::fuse_gyro_only(quaternion& rotation, float dt, const vector& angular_velocity,
  const vector& acceleration, const vector& magnetic_field)
{
    rotate(rotation, angular_velocity, dt);
}

void ahrs::fuse_default(quaternion& rotation, float dt, const vector& angular_velocity,
  const vector& acceleration, const vector& magnetic_field)
{
    vector correction = vector(0, 0, 0);

    if (abs(acceleration.norm() - 1) <= 0.3)
    {
        // The magnetidude of acceleration is close to 1 g, so
        // it might be pointing up and we can do drift correction.

        const float correction_strength = 1;

        matrix rotationCompass = rotationFromCompass(acceleration, magnetic_field);
        matrix rotationMatrix = rotation.toRotationMatrix();

        correction = (
            rotationCompass.row(0).cross(rotationMatrix.row(0)) +
            rotationCompass.row(1).cross(rotationMatrix.row(1)) +
            rotationCompass.row(2).cross(rotationMatrix.row(2))
          ) * correction_strength;

    }

    rotate(rotation, angular_velocity + correction, dt);
}

void ahrs::fuse_raw(quaternion& rotation, float dt, const vector& angular_velocity,
  const vector& acceleration, const vector& magnetic_field)
{
//    imu.read();
    printf("%7d %7d %7d  %7d %7d %7d  %7d %7d %7d\n",
           mImu.raw_m[0], mImu.raw_m[1], mImu.raw_m[2],
           mImu.raw_a[0], mImu.raw_a[1], mImu.raw_a[2],
           mImu.raw_g[0], mImu.raw_g[1], mImu.raw_g[2]
           );
}
