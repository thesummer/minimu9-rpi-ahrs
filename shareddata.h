#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include<vector.h>


class sharedData
{
public:
    struct gpsData
    {
        float longitude;
        float latitude;
        float height;
    };

    sharedData();

    quaternion getRotation();
    void setRotation(quaternion rotation);

    gpsData  getGpsData();
    void setGpsData(gpsData newGps);
    bool newGpsData() { return mNewGpsData; }


private:
    quaternion mRotation;
    pthread_mutex_t mMutexRotation;

    struct gpsData mGpsData;
    pthread_mutex_t mMutexGPS;
    bool mNewGpsData;



};

#endif // SHAREDDATA_H
