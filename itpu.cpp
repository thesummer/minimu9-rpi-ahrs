#include <itpu.h>

#include<shareddata.h>
#include<ahrs.h>
#include<gpsetag.h>
#include <camera.h>

#include<iostream>
#include<vector.h>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <boost/program_options.hpp>
#include <time.h>
#include <sys/time.h>

namespace opts = boost::program_options;


sharedData *sd;
static camera *cam;
static gpsEtag *etag;
static ahrs *attitude;


// An Euler angle could take 8 chars: -234.678, but usually we only need 6.
float field_width = 6;
#define FLOAT_FORMAT std::fixed << std::setprecision(3) << std::setw(field_width)

std::ostream & operator << (std::ostream & os, const vector & vector)
{
    return os << FLOAT_FORMAT << vector(0) << ' '
              << FLOAT_FORMAT << vector(1) << ' '
              << FLOAT_FORMAT << vector(2);
}

std::ostream & operator << (std::ostream & os, const matrix & matrix)
{
    return os << (vector)matrix.row(0) << ' '
              << (vector)matrix.row(1) << ' '
              << (vector)matrix.row(2) << std::endl;
}

std::ostream & operator << (std::ostream & os, const quaternion & quat)
{
    return os << FLOAT_FORMAT << quat.w() << ' '
              << FLOAT_FORMAT << quat.x() << ' '
              << FLOAT_FORMAT << quat.y() << ' '
              << FLOAT_FORMAT << quat.z();
}


typedef void rotation_output_function(quaternion& rotation);

void output_quaternion(quaternion & rotation)
{
    std::cout << rotation;
}

void output_matrix(quaternion & rotation)
{
    std::cout << rotation.toRotationMatrix();
}

void output_euler(quaternion & rotation)
{
//    std::cout << (vector)(rotation.toRotationMatrix().eulerAngles(2, 1, 0) * (180 / M_PI));
    vector temp = (vector)(rotation.toRotationMatrix().eulerAngles(2, 1, 0) * (180 / M_PI));
    std::cout << "!ANG:" << temp(2) << "," << temp(1) << "," << temp(0) << std::endl;
}

void output_raw(quaternion & rotation)
{
    ;
}

int itpuInit(int argc, char *argv[])
{
    try
    {
        // Define what all the command-line parameters are.
        std::string mode, output_mode, i2cDevice, serialDevice;
        opts::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("i2c-bus,b", opts::value<std::string>(&i2cDevice)->default_value("/dev/i2c-0"),
                 "i2c-bus the IMU is connected to")
                ("serDev,s", opts::value<std::string>(&serialDevice)->default_value("/dev/ttyO1"),
                 "serial device the ETAG is connected to")
                ("mode", opts::value<std::string>(&mode)->default_value("normal"),
                 "specifies what algorithm to use.\n"
                 "normal: Fuse compass and gyro.\n"
                 "gyro-only:  Use only gyro (drifts).\n"
                 "compass-only:  Use only compass (noisy).\n"
                 "raw: Just print raw values from sensors.")
                ("output", opts::value<std::string>(&output_mode)->default_value("matrix"),
                 "specifies how to output the orientation.\n"
                 "matrix: Direction Cosine Matrix.\n"
                 "quaternion: Quaternion.\n"
                 "euler: Euler angles (yaw, pitch, roll).\n")
                ;
        opts::variables_map options;
        opts::store(opts::command_line_parser(argc, argv).options(desc).run(), options);
        opts::notify(options);

        if(options.count("help"))
        {
            std::cout << desc << std::endl;
            std::cout << "For more information, run: man minimu9-ahrs" << std::endl;
            return 0;
        }

        sd   = new sharedData();
        attitude = new ahrs(*sd, i2cDevice.c_str());
        etag = new gpsEtag(*sd, serialDevice);
        cam  = new camera(*sd, "./cam.log");

        rotation_output_function * output;

        // Figure out the output mode.
        if (output_mode == "matrix")
        {
            output = &output_matrix;
        }
        else if (output_mode == "quaternion")
        {
            output = &output_quaternion;
        }
        else if (output_mode == "euler")
        {
            field_width += 2;  // See comment above for field_width.
            output = &output_euler;
        }
        else
        {
            std::cerr << "Unknown output mode '" << output_mode << "'" << std::endl;
            return 1;
        }

        // Figure out the basic operating mode and start running.
        if (mode == "raw")
        {
            attitude->setFuseType(ahrs::raw);
            output = &output_raw;
        }
        else if (mode == "gyro-only")
        {
           attitude->setFuseType(ahrs::gyro_only);
        }
        else if (mode == "compass-only")
        {
            attitude->setFuseType(ahrs::compass_only);
        }
        else if (mode == "normal")
        {
            attitude->setFuseType(ahrs::normal);
        }
        else
        {
            std::cerr << "Unknown mode '" << mode << "'" << std::endl;
            return 1;
        }

        return 0;
    }
    catch(const std::system_error & error)
    {
        std::string what = error.what();
        const std::error_code & code = error.code();
        std::cerr << "Error: " << what << "  " << code.message() << " (" << code << ")" << std::endl;
        return 2;
    }
    catch(const opts::multiple_occurrences & error)
    {
        std::cerr << "Error: " << error.what() << " of " << error.get_option_name() << " option." << std::endl;
        return 1;
    }
    catch(const std::exception & error)
    {
        std::cerr << "Error: " << error.what() << std::endl;
        return 9;
    }
}

void itpuStart()
{
    attitude->start();
    etag->start();
}

void getEulerAngle(float angles[3])
{
    vector temp = (vector)(sd->getRotation().toRotationMatrix().eulerAngles(2, 1, 0) * (180 / M_PI));
    angles[0] = temp(0);
    angles[1] = temp(1);
    angles[2] = temp(2);
}

void setCameraInterval(int seconds)
{
    cam->setCaptureInterval(seconds);
}

void startCamera()
{
    cam->start();
}

void stopCamera()
{
    cam->stop();
}

struct gpsData getGps()
{
    return sd->getGpsData();
}
