#ifndef DMXBULB_H
#define DMXBULB_H

#include "Bulb.h"

class DMXBulb : public Bulb
{
public:
    DMXBulb(unsigned char * pos);
    ~DMXBulb();
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

#endif // DMXBULB_H
