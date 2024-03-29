#include "FadeableBulb.h"
#include <stdio.h>

FadeableBulb::FadeableBulb()
{
    //ctor
    fadeStep = 0;
    fadeStop = 0;
    percentage = 0;
    this->next = 0;
    this->next = firstBulb;
    firstBulb = this;

}

FadeableBulb *FadeableBulb::firstBulb = 0;


FadeableBulb::~FadeableBulb()
{
    //dtor
}

void FadeableBulb::setDebug(bool d)
{
    this->debug = d;
    printf("Setting Debug on Fadebulb %d\n", this->debug);
}

/**
* duration is in seconds
*/
void FadeableBulb::fade(int start, int stop, double duration)
{
    start = start < 0 ? 0 : start;
    start = start > 100 ? 100 : start;
    stop = stop < 0 ? 0 : stop;
    stop = stop > 100 ? 100 : stop;

    setIntensity(start);
    this->percentage = start;
    fadeStep = ((double)(stop-start)) / (duration*3000); // not sure by 3000 is needed, but it works
    fadeStop = stop;


    if (this->debug)
    {
        printf("DEBUG Fade: Start: %d, Stop: %d, duration %f, fadeStep: %f\n",start,stop,duration,fadeStep);
    }


    //printf("FadeStep: %f\n", fadeStep );
}

void FadeableBulb::setIntensity(int pct)
{
    //printf("Fadeable Bulb SetIntensity\n");
    fadeStep = 0;
    fadeStop = 0;
    this->percentage = pct;
    Bulb::setIntensity(pct);
}


void * FadeableBulb::tickThread(void *)
{
    auto begin = std::chrono::high_resolution_clock::now() ;
    while (1)
    {
        usleep(25 * 1000); // 25ms)
        auto end = std::chrono::high_resolution_clock::now() ;
        auto ticks = std::chrono::duration_cast<std::chrono::microseconds>(end-begin) ;

        double duration = ((double)ticks.count())/1000;

        FadeableBulb *current = 0;
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

void FadeableBulb::fadeTick(double duration)
{
    if (fadeStep == 0)
        return;
    //printf("P1: %f, Fadestep:  %f \n", percentage, fadeStep);
    this->percentage += (fadeStep * duration);
    //printf("P2: %f\n", percentage);
    if (percentage < 0)
    {
        percentage = 0;
        fadeStep = 0;
    }
    if (percentage > 100)
    {
        percentage = 100;
        fadeStep = 0;
    }
    if (fadeStep > 0 && percentage >= fadeStop)
    {
        //printf("Stop 1 %f   %f   %d\n  ", fadeStep, percentage, fadeStop);
        fadeStep = 0;
        fadeStop = 0;
    }
    if (fadeStep < 0 && percentage <= fadeStep)
    {
        //printf("Stop 2\n");
        fadeStep = 0;
        fadeStop = 0;
    }

/*
    if (this->debug)
    {
        printf("Tick: fadeStep: %f, pct: %f, Duration: %f\n", fadeStep,percentage, duration);
    }
*/
    Bulb::setIntensity(percentage);
}

