#ifndef GPSDATA_H
#define GPSDATA_H

#include <nmea/time.h>

struct gpsData
{
    float longitude;
    float latitude;
    float height;
    nmeaTIME utc;
};

#endif // GPSDATA_H
