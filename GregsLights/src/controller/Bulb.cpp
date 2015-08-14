/*
 * Defines a single Bulb that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

#include "../../include/controller/Bulb.h"
#include <stdio.h>
#include <chrono>

int Bulb::getIntensity()
{
    return this->pct;
}

Bulb::Bulb()
{
    this->pct = 0;
}


/**
* duration is in seconds
*/
void Bulb::fade(int start, int stop, double duration)
{
    setIntensity(stop);
    throw "Bulb:fade was called. Should never happen";
}

void Bulb::fadeTo(int stop, double duration)
{
    this->fade(getIntensity(),stop,duration);
}



void Bulb::setIntensity(int newpct)
{
    int value = 0;
    if (newpct > 100)
    {
        value = getMax();
        this->pct = 100;
    }
    else if (newpct < 0)
    {
        value = getMin();
        this->pct = 0;
    }
    else
    {
        value = ((getMax() - getMin()) * newpct)/100;
        this->pct = newpct;
    }

    setIntensity_ipml(value);
}
