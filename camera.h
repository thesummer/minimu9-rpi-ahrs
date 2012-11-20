#ifndef CAMERA_H
#define CAMERA_H

#include <shareddata.h>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>

class camera
{
public:
    camera(sharedData &sD, std::string filename);
    bool singleShot();

    bool start();
    bool stop();

    void setCaptureInterval(int seconds);
    int getCaptureInterval() { return mInterval;}

private:
    sharedData& mSharedData; //reference
    std::fstream mFile;
    cv::VideoCapture mCap;
    int mInterval;
    int mImageCount;

    pthread_t mThread;
    bool mRunning;
    static void*  callThreadFunction(void *arg) { return ((camera*)arg)->threadCamera(); }

    void * threadCamera();

    bool takePicture();
};

#endif // CAMERA_H
