#ifndef OPENDMXNETWORK_H
#define OPENDMXNETWORK_H

#include "IPixal.h"
#include "RGBLight.h"


class OpenDMXNetwork
{
public:
    OpenDMXNetwork(char * deviceName);
    RGBLight* getRGB(int start);
    IPixal* getPixal(int channel);
    virtual ~OpenDMXNetwork();
    void doUpdate(long ms);
protected:
private:
    unsigned char data[513];
};

class DMXPixal : public IPixal
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
