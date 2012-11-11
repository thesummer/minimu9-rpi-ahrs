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
