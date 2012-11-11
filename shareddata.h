#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include<vector.h>


class sharedData
{
public:
    sharedData();

    quaternion getRotation();
    void setRotation(quaternion rotation);
//    getGPS();


private:
    quaternion mRotation;
    pthread_mutex_t mMutexRotation;

//    GPS
//    pthread_mutex_t mMutexGPS;


};

#endif // SHAREDDATA_H
