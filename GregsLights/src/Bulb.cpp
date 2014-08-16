/*
 * Defines a single Bulb that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

#include "../include/Bulb.h"
#include <stdio.h>

void Bulb::setIntensity(int pct)
{
    int value = 0;
    if (pct > 100)
    {
        setIntensity_ipml(getMax());
        printf("WARNING: Intensity > 100%% : %d\n", pct);
    }
    else if (pct < 0)
    {
        printf("WARNING: Intensity < 0%% : %d\n", pct);
        setIntensity_ipml(getMin());
    }
    else
    {
        value = ((getMax() - getMin()) * pct)/100;
        setIntensity_ipml(value);
    }
}
