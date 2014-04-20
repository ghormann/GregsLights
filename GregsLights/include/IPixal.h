#ifndef IPIXAL_H
#define IPIXAL_H

/*
 * Defines a single light (or pixal) that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

class IPixal
{
public:
    void setIntensity(int pct);
protected:
    virtual void setIntensity_ipml(int pct) = 0;
    virtual int getMin() = 0;
    virtual int getMax() = 0;

private:
};

#endif // IPIXAL_H
