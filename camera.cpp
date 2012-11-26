#include "camera.h"

#include <iostream>
#include <iomanip>

#include <thread.h>
#include <vector.h>
#include <shareddata.h>
#include <gpsData.h>

camera::camera(sharedData& sD, std::string filename):
     mSharedData(sD), mFile(filename, std::fstream::out), mImageCount(0), mRunning(false)
{
    mInterval = 1;
    mCap.setDesiredSize(1280, 1024);
    mCap.open();

    if(!mCap.isOpen())
        std::cout << "Opening video device failed" << std::endl;

}

bool camera::singleShot()
{
    return takePicture();
}

bool camera::start()
{
    if (mRunning)
    {
        return false;
    }

    pthread_create (&mThread, NULL, camera::callThreadFunction, this) ;

    mRunning = true;
    return true;
}

bool camera::stop()
{
    if(!mRunning)
    {
        return false;
    }

    mRunning = false;
    pthread_join(mThread, NULL);

    return true;
}

void camera::setCaptureInterval(int seconds)
{
    mInterval = seconds;
    stop();
    start();
}

void *camera::threadCamera()
{
    struct periodic_info info;
    makePeriodic(mInterval*1000000, &info);

    while(1)
    {
        if(!mRunning)
        {
            pthread_exit(NULL);
        }

        takePicture();
        waitPeriod(&info);
    }
    return NULL;
}

bool camera::takePicture()
{
    cv::Mat frame;
    if (!mCap.grab())
    {
        std::cout << "Capturing failed" << std::endl;
    }

    mCap.rgb(frame);
    struct gpsData gps = mSharedData.getGpsData();

    std::ostringstream fileName;
    fileName << std::right;


    fileName.fill('0');
    fileName << "./image" << std::setw(4) << mImageCount << ".jpg";
    cv::imwrite(fileName.str(), frame);
    mImageCount++;

    // Add metainfo to log file
    matrix rot = mSharedData.getRotation().toRotationMatrix();
    // Date and Time
    mFile << gps.utc.year << '-' << gps.utc.mon << '-' << gps.utc.day
          << '-' << gps.utc.hour << ':' << gps.utc.min << ':' << gps.utc.sec << '\t';
    // Filename and rotation
    mFile << fileName.str() << '\t' << rot.row(0) << rot.row(1) << rot.row(2) << '\t';
    mFile << rot.eulerAngles(2, 1, 0).transpose() * (180 / M_PI) << '\t';
    // Gps position
    mFile << gps.latitude << '\t' << gps.longitude << '\t' << gps.height << std::endl;

    return true;
}
