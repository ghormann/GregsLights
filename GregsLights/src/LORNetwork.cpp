#include "../include/LORNetwork.h"
#include "../include/Bulb.h"
#include "time.h"
#include <iostream>
#include <stdio.h>

LORNetwork::LORNetwork(char * deviceName)
{
    clock_gettime(CLOCK_MONOTONIC_RAW, &last_ts);
    last_ts.tv_sec -= 3;  // Subtrack 3 seconds to force update.

    char errmsg[100];
    serptr=new SerialPort();
    int errcode=serptr->Open(deviceName, 115200, "8N1");
    if (errcode < 0)
    {
        sprintf(errmsg,"unable to open serial port %s, error code=%d", deviceName, errcode);
        printf("ERROR: %s", errmsg);
        throw errmsg;
    }

}

LORNetwork::~LORNetwork()
{
    //dtor
}

void LORNetwork::doUpdate()
{
    using namespace std;
    struct timespec ts;
    long diff = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    diff = (ts.tv_sec - last_ts.tv_sec) * 1000LL + ((ts.tv_nsec - last_ts.tv_nsec) /1000000LL);
    if ((diff < 0) || diff > 400) //400 ms
    {

        unsigned char msg[10];
        msg[0] = 0;
        msg[1] = 0xFF;
        msg[2] = 0x81;
        msg[3] = 0x56;
        msg[4] = 0;
        if (serptr)
            serptr->Write((char *)msg,5);

#ifdef GJH_DEBUG
        cout << "Send Heartbeat " << diff  << " " << (ts.tv_sec - last_ts.tv_sec) << endl;
#endif
        last_ts.tv_nsec = ts.tv_nsec;
        last_ts.tv_sec = ts.tv_sec;
    }
}

Bulb* LORNetwork::getBulb(int device, int channel)
{
    if (device < 1 || device > 127)
        throw "Invalid Device ID";

    if (channel < 1 || channel > 16)
        throw "Invalid Channel ID";

    LORBulb *rc =  new LORBulb((unsigned char) device, (unsigned char) channel, this);
    return rc;

}


LORBulb::LORBulb(unsigned char device, unsigned char channel, LORNetwork *network)
{
    this->device = device;
    this->channel = channel;
    this->network = network;
}

void LORBulb::setIntensity_ipml(int pct)
{
    unsigned char msg[10];
    msg[0] = 0;
    msg[1] = device;
    msg[2] = 3;
    msg[3] = (unsigned char) convertIntensity(pct);
    msg[4] = channel + 127;
    msg[5] = 0;

    if (network->serptr)
        network->serptr->Write((char *)msg,6);


#ifdef GJH_DEBUG
    char greg[20]; // Debug ONly
    for (int j = 0; j < 6; j++)
    {
        sprintf(greg+2*j, "%02X", msg[j]);
    }
    greg[12] = '\0';


    printf("LOR: %s\n", greg);
#endif

}
