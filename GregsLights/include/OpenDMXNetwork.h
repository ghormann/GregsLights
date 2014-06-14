#ifndef OPENDMXNETWORK_H
#define OPENDMXNETWORK_H

#include "Bulb.h"
#include "RGBLight.h"
#include "LightNetwork.h"
#include "../include/serial.h"


class OpenDMXNetwork : public LightNetwork
{
public:
    OpenDMXNetwork(char * deviceName);
    RGBLight* getRGB(int start);
    Bulb* getBulb(int channel);
    void doUpdate();
    virtual ~OpenDMXNetwork();
protected:
private:
    unsigned char data[513];
    SerialPort* serptr;
};

class DMXBulb : public Bulb
{
public:
    DMXBulb(unsigned char * pos)
    {
        this->pos = pos;
    }
protected:
    void setIntensity_ipml(int pct);
    int getMin()
    {
        return 0;
    }
    int getMax()
    {
        return 255;
    }
private:
    unsigned char* pos;
};


#endif // OPENDMXNETWORK_H
