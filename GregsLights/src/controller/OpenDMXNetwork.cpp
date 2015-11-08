#include "../../include/controller/OpenDMXNetwork.h"
#include <stdio.h>
#include <unistd.h>

/*
 * A network of DMX controls that are on channels 0 - 512 and using
 * serial dervice with bauild of 250000 and 8N2 serial protocol if OpenDX
 * or 8N1 if ActiDongle
 */
OpenDMXNetwork::OpenDMXNetwork(char * deviceName,RGB_TYPE type, bool sendData)
{
    serptr = 0;   // Make sure it defaults to null pointer.
    offset = 0;
    for (int i =0; i <513; i++)
    {
        this->data[i] = 0;
    }
    char errmsg[100];
    int errcode;

    if (sendData)
    {
        if (type == OPENDMX)
        {
            serptr=new SerialPort();
            errcode=serptr->Open(deviceName, 250000, "8N2");
        }
        else
        {
            serptr=new SerialPort();
            errcode=serptr->Open(deviceName, 250000, "8N1");
            this->offset = 4;
            data[0]=0x7E;               // start of message
            data[1]=6;                  // dmx send
            data[2]= 513& 0xFF;         // length LSB
            data[3]=(513 >> 8) & 0xFF;  // length MSB
            data[4]=0;                  // DMX start
            data[512 + this->offset]=0xE7;       // end of message
        }

        if (errcode < 0)
        {
            sprintf(errmsg,"unable to open serial port %s, error code=%d", deviceName, errcode);
            printf("ERROR: %s", errmsg);
            throw errmsg;
        }
    }
//printf("Opened: %s\n", deviceName);
}

void OpenDMXNetwork::setShutdown(bool val)
{
    // for now, do Nothing.
}

Bulb* OpenDMXNetwork::getBulb(int channel)
{
    if (channel < 0 || channel > 512)
        throw "Invalid Channel";
    DMXBulb *rc = new DMXBulb(data+channel+offset);
    return  rc;
}

/*
 * THis function must be called on a timer to preform update
 */
bool OpenDMXNetwork::doUpdate(bool force) // force is ignored
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

    if (serptr) // makesure Created
    {
        serptr->SendBreak();  // sends a 1 millisecond break
        usleep(1000);      // mark after break (MAB) - 1 millisecond is overkill (8 microseconds is the minimum dmx requirement)
        serptr->Write((char *)data,513+offset);
    }
    return true;

}

/*
 * Returns an RGBLight that will right to the 3 consenstve channels
 * starting with the start channel
 */
RGBLight* OpenDMXNetwork::getRGB(int start)
{
    Bulb *r = this->getBulb(start);
    Bulb *g = this->getBulb(start+1);
    Bulb *b = this->getBulb(start+2);
    return new RGBLight(r,g,b);
}

OpenDMXNetwork::~OpenDMXNetwork()
{
    if (serptr) delete serptr;
}


