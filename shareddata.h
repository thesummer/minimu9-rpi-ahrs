#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include<vector.h>
#include<nmea/time.h>
#include<gpsData.h>


class sharedData
{
public:
    sharedData();

    quaternion getRotation();
    void setRotation(quaternion rotation);

    struct gpsData getGpsData();
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
