/*
 * Defines a single Bulb that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

#include "../../include/controller/Bulb.h"
#include <stdio.h>

int Bulb::getIntensity()
{
    return this->percentage;
}

void Bulb::setIntensity(int pct)
{
    int value = 0;
    if (pct > 100)
    {
        value = getMax();
        this->percentage = 100;
    }
    else if (pct < 0)
    {
        value = getMin();
        this->percentage = 0;
    }
    else
    {
        value = ((getMax() - getMin()) * pct)/100;
        this->percentage = pct;
    }

    setIntensity_ipml(value);
}
