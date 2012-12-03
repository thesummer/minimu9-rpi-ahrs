#include "shareddata.h"

sharedData::sharedData():
    mNewGpsData(false)
{
     pthread_mutex_init(&mMutexRotation, NULL);
     pthread_mutex_init(&mMutexGPS, NULL);     
     gpsData temp;
     temp.height = 0;
     temp.latitude = 0;
     temp.longitude = 0;

     temp.utc.year = 1996;
     temp.utc.mon = 12;
     temp.utc.day = 22;
     temp.utc.hour = 12;
     temp.utc.min = 55;
     temp.utc.sec = 0;
     temp.utc.hsec = 0;

     setGpsData(temp);
}

quaternion sharedData::getRotation()
{
     quaternion temp;
     pthread_mutex_lock(&mMutexRotation);
        temp = mRotation;
     pthread_mutex_unlock(&mMutexRotation);

     return temp;
}

void sharedData::setRotation(quaternion rotation)
{
    pthread_mutex_lock(&mMutexRotation);
       this->mRotation = rotation;
    pthread_mutex_unlock(&mMutexRotation);
}

struct gpsData sharedData::getGpsData()
{
    gpsData temp;
    pthread_mutex_lock(&mMutexGPS);
       temp = mGpsData;
    pthread_mutex_unlock(&mMutexGPS);

    mNewGpsData = false;
    return temp;
}

void sharedData::setGpsData(gpsData newGps)
{
    pthread_mutex_lock(&mMutexGPS);
       this->mGpsData = newGps;
    pthread_mutex_unlock(&mMutexGPS);

    mNewGpsData = true;
}
