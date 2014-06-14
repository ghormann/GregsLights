#ifndef OPENDMXNETWORK_H
#define OPENDMXNETWORK_H

#include "Bulb.h"
#include "RGBLight.h"
#include "../include/serial.h"


class OpenDMXNetwork
{
public:
    OpenDMXNetwork(char * deviceName);
    RGBLight* getRGB(int start);
    Bulb* getPixal(int channel);
    virtual ~OpenDMXNetwork();
    void doUpdate();
protected:
private:
    unsigned char data[513];
    SerialPort* serptr;
};

class DMXPixal : public Bulb
{
public:
    DMXPixal(unsigned char * pos)
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
