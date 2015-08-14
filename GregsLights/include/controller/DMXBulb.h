#ifndef DMXBULB_H
#define DMXBULB_H

#include "FadeableBulb.h"

class DMXBulb : public FadeableBulb
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
