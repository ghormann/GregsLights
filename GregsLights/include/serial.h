// Name:        serial.h
// Author:      Matt Brown

#ifndef XLIGHTS_SERIAL_H
#define XLIGHTS_SERIAL_H

#include <termios.h>
#include <cstddef>

class xNetwork_Renard;

class SerialPort
{
protected:
    char * m_devname;
    int fd;
    int callback;
    struct termios t;
    speed_t AdaptBaudrate( int baud );

public:
    SerialPort();
    ~SerialPort();
    void SetCallback(int cb);
    int GetCallback();
    int AvailableToRead();
    int WaitingToWrite();
    int SendBreak();
    int Close();
    int Open( char* portname, int baudrate, const char* protocol = "8N1" );
    bool IsOpen();
    int Read(char* buf,size_t len);
    int Write(char* buf,size_t len);
};

#endif // XLIGHTS_SERIAL_H
