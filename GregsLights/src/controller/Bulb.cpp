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
    return this->percentage;
}

Bulb::Bulb()
{
    fadeStep = 0;
    fadeStop = 0;
    percentage = 0;
    this->next = firstBulb;
    firstBulb = this;
}

Bulb *Bulb::firstBulb = 0;

/**
* duration is in milliseconds
*/
void Bulb::fade(int start, int stop, double duration)
{
    start = start < 0 ? 0 : start;
    start = start > 100 ? 100 : start;
    stop = stop < 0 ? 0 : stop;
    stop = stop > 100 ? 100 : stop;

    setIntensity(start);
    fadeStep = ((double)(stop-start)) / (duration*1000);
    fadeStop = stop;

    //printf("FadeStep: %f\n", fadeStep );
}

void * Bulb::tickThread(void *)
{
    auto begin = std::chrono::high_resolution_clock::now() ;
    while (1)
    {
        usleep(50 * 1000); // 50ms)
        auto end = std::chrono::high_resolution_clock::now() ;
        auto ticks = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin) ;

        double duration = ticks.count();

        Bulb *current = 0;
        current = firstBulb;
        while (current != 0)
        {
            current->fadeTick(duration);
            current = current->next;
        }
        begin = end; // Save for next time
    }

    return NULL;
}

void Bulb::fadeTick(double duration)
{
    if (fadeStep == 0)
        return;
    //printf("P1: %f\n", percentage);
    this->percentage += (fadeStep * duration);
    //printf("P2: %f\n", percentage);
    if (percentage < 0)
    {
        percentage = 0;
    }
    if (percentage > 100)
    {
        percentage = 100;
    }
    if (fadeStep > 0 && percentage >= fadeStop)
    {
        //printf("Stop 1 %f   %f   %d\n  ", fadeStep, percentage, fadeStop);
        fadeStep = fadeStop = 0;
    }
    if (fadeStep < 0 && percentage <= fadeStep)
    {
        //printf("Stop 2\n");
        fadeStep = fadeStop = 0;
    }

    int value = ((getMax() - getMin()) * percentage)/100;
    setIntensity_ipml(value);
}


void Bulb::setIntensity(int pct)
{
    // If User called then setting Intensity then fade is over
    fadeStep = 0;

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
        this->percentage = (double) pct;
    }

    setIntensity_ipml(value);
}
