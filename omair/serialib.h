/*!
 * \file    serialib.h
 * \brief   Serial library to communicate throught serial port, or any device emulating a serial port.
 * \author  Philippe Lucidarme
 * \version 1.1
 * \date    28 avril 2011
 *
 * This Serial library is used to communicate through serial port.
 *
 */


#ifndef SERIALIB_H
#define SERIALIB_H


// Include for Linux
#ifdef __linux__
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <termios.h>
#include <string.h>
//#include <iostream.h>
#include <fcntl.h>                                  // File control definitions
#include <stdio.h>
#include "timer.h"
#endif



/*!  \Functions for serial port
     
     \example   Example1.cpp
   */




    // ::: Configuration and initialization :::

    char    Open        (const char *Device,const unsigned int Bauds);      // Open a device
    void    Close();                                                        // Close the device

    // ::: Read/Write operation on characters :::

    char    WriteChar   (char);                                             // Write a char
    char    ReadChar    (char *pByte,const unsigned int TimeOut_ms);         // Read a char (with timeout)

    // ::: read/Write operation on strings :::

    char    WriteString (const char *String);                               // Write a string
    int     ReadString  (char *String,char FinalChar,unsigned int MaxNbBytes,const unsigned int TimeOut_ms); // Read a string (with timeout)

    // ::: Read/Write operation on bytes :::

    char    Write       (const void *Buffer, const unsigned int NbBytes); // Write an array of bytes
    int     Read        (void *Buffer,unsigned int MaxNbBytes,const unsigned int TimeOut_ms);



    int     ReadStringNoTimeOut  (char *String,char FinalChar,unsigned int MaxNbBytes);             // Read a string (no timeout)


#endif // SERIALIB_H

