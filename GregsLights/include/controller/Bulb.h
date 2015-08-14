#ifndef BULB_H
#define BULB_H
#include <chrono>
#include <unistd.h>

#define gjhSleep(pause)  usleep((pause) * 1000000)

/*
 * Defines a single light (or pixal) that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

class Bulb
{
public:
    Bulb();
    void setIntensity(int pct);
    int  getIntensity();
    virtual void fade(int start, int stop, double duration);
    void fadeTo(int stop, double duration);
protected:
    virtual void setIntensity_ipml(int pct) = 0;
    virtual int getMin() = 0;
    virtual int getMax() = 0;

private:
    char pct;
};

#endif // IPIXAL_H
