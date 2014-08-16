#ifndef BULB_H
#define BULB_H

/*
 * Defines a single light (or pixal) that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

class Bulb
{
public:
    void setIntensity(int pct);
    int  getIntensity();
protected:
    virtual void setIntensity_ipml(int pct) = 0;
    virtual int getMin() = 0;
    virtual int getMax() = 0;

private:
    int percentage;
};

#endif // IPIXAL_H
