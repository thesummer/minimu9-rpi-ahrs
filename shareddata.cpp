#include "shareddata.h"

sharedData::sharedData()
{
     pthread_mutex_init(&mMutexRotation, NULL);
//     pthread_mutex_init(&mMutexGPS, NULL);
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

sharedData::gpsData sharedData::getGpsData()
{
    gpsData temp;
    pthread_mutex_lock(&mMutexGPS);
       temp = mGpsData;
    pthread_mutex_unlock(&mMutexGPS);

    mNewGpsData = false;
    return temp;
}

void sharedData::setGpsData(sharedData::gpsData newGps)
{
    pthread_mutex_lock(&mMutexGPS);
       this->mGpsData = newGps;
    pthread_mutex_unlock(&mMutexGPS);

    mNewGpsData = true;
}
