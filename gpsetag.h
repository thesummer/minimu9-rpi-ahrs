#ifndef GPSETAG_H
#define GPSETAG_H

#include<shareddata.h>
#include<stdlib.h>

class gpsEtag
{
public:
    gpsEtag(sharedData& sD, std::string serialDev);

    bool start();
    bool stop();

private:
    sharedData& mSharedData; //reference
    pthread_t mThread;
    std::string mSerialDevice;
    bool mRunning;

    static void*  callThreadFunction(void *arg) { return ((gpsEtag*)arg)->threadGps(); }
    void * threadGps();
};

#endif // GPSETAG_H
