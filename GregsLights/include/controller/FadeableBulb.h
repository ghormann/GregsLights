#ifndef FADEABLEBULB_H
#define FADEABLEBULB_H

#include <Bulb.h>


class FadeableBulb : public Bulb
{
public:
    FadeableBulb();
    virtual ~FadeableBulb();
    static void * tickThread(void *);
    virtual void fade(int start, int stop, double duration);
    void setIntensity(int pct);
protected:
private:
    static FadeableBulb *firstBulb;
    void fadeTick(double duration);
    FadeableBulb *next = 0;
    float fadeStep = 0;  /* Per ms */
    float percentage = 0;
    char fadeStop = 0; // range 0-100


};

#endif // FADEABLEBULB_H
