#ifndef DUMMYBULB_H
#define DUMMYBULB_H

#include "Bulb.h"

class DummyBulb : public Bulb
{
    public:
        DummyBulb();
        virtual ~DummyBulb();
    protected:
    virtual void setIntensity_ipml(int pct);
    virtual int getMin();
    virtual int getMax();
    private:
};

#endif // DUMMYBULB_H
