#include "../../include/controller/DMXBulb.h"


DMXBulb::DMXBulb(unsigned char * pos) : FadeableBulb()
    {
        this->pos = pos;
    }

DMXBulb::~DMXBulb()
{
    //dtor
}

void DMXBulb::setIntensity_ipml(int val)
{
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    pos[0] = (char) val;
}

