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
        setIntensity_ipml(getMax());
        printf("WARNING: Intensity > 100%% : %d\n", pct);
        this->percentage = 100;
    }
    else if (pct < 0)
    {
        printf("WARNING: Intensity < 0%% : %d\n", pct);
        setIntensity_ipml(getMin());
        this->percentage = 0;
    }
    else
    {
        value = ((getMax() - getMin()) * pct)/100;
        setIntensity_ipml(value);
        this->percentage = pct;
    }
}
