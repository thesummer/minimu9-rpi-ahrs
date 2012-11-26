#include "gpsetag.h"
#include <math.h>
#include <SerialPort.h>
#include <iostream>
#include <thread.h>
#include <pthread.h>

#include<nmea/nmea.h>

gpsEtag::gpsEtag(sharedData& sD, std::string serialDev):
    mSharedData(sD), mSerialDevice(serialDev), mRunning(false)
{    

}

bool gpsEtag::start()
{
    if (mRunning)
    {
        return false;
    }

    pthread_create (&mThread, NULL, gpsEtag::callThreadFunction, this) ;

    mRunning = true;
    return true;
}

bool gpsEtag::stop()
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

void *gpsEtag::threadGps()
{
    SerialPort serialPort(mSerialDevice.c_str());
    serialPort.Open(SerialPort::BAUD_2400);

    nmeaINFO gpsInfo;
    nmeaPARSER parser;
    nmeaPOS dpos;
    nmea_zero_INFO(&gpsInfo);
    if (nmea_parser_init(&parser) == 0)
        std::cout << "ERROR while creating parser" << std::endl;


    while(1)
    {
        std::string temp = serialPort.ReadLine();

        nmea_parse(&parser, temp.c_str(), temp.length(), &gpsInfo);
        nmea_info2pos(&gpsInfo, &dpos);

        sharedData::gpsData gps;
        gps.latitude  = dpos.lat * 180 / M_PI;
        gps.longitude = dpos.lon * 180 / M_PI;
        gps.height    = gpsInfo.elv;
        gps.utc       = gpsInfo.utc;

        mSharedData.setGpsData(gps);
    }
    return (void*) NULL;

}
