#include "../include/OpenDMXNetwork.h"
#include <stdio.h>
#include <unistd.h>

OpenDMXNetwork::OpenDMXNetwork(char * deviceName)
{
    for (int i =0; i <513; i++)
    {
        this->data[i] = 0;
    }
    char errmsg[100];
    serptr=new SerialPort();
    int errcode=serptr->Open(deviceName, 250000, "8N2");
    if (errcode < 0)
    {
        sprintf(errmsg,"unable to open serial port %s, error code=%d", deviceName, errcode);
        printf("ERROR: %s", errmsg);
        throw errmsg;
    }
    //printf("Opened: %s\n", deviceName);
}

Bulb* OpenDMXNetwork::getPixal(int channel)
{
    if (channel < 0 || channel > 512)
        throw "Invalid Channel";
    DMXPixal *rc = new DMXPixal(data+channel);
    return  rc;
}

void OpenDMXNetwork::doUpdate()
{
#ifdef GJH_DEBUG
    char greg[600];
    for (int j = 0; j < 10; j++)
    {
        sprintf(greg+2*j, "%02X", data[j]);
        greg[20] = '\0';
    }
    printf("Data: %s\n", greg);
#endif

    if (serptr)
    {
        serptr->SendBreak();  // sends a 1 millisecond break
        usleep(1000);      // mark after break (MAB) - 1 millisecond is overkill (8 microseconds is the minimum dmx requirement)
        serptr->Write((char *)data,513);
    }

}

RGBLight* OpenDMXNetwork::getRGB(int start)
{
    Bulb *r = this->getPixal(start);
    Bulb *g = this->getPixal(start+1);
    Bulb *b = this->getPixal(start+2);
    return new RGBLight(r,g,b);
}

OpenDMXNetwork::~OpenDMXNetwork()
{
    if (serptr) delete serptr;
}

void DMXPixal::setIntensity_ipml(int val)
{
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    pos[0] = (char) val;
}
