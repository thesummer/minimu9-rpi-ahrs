#ifndef ITPU_H
#define ITPU_H
#include<gpsData.h>

int itpuInit(int argc, char *argv[]);

void itpuStart();

void getEulerAngle(float angles[3]);

void setCameraInterval(int seconds);

void startCamera();

void stopCamera();

struct gpsData getGps();

#endif // ITPU_H
