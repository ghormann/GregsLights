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
protected:
private:
    float fadeStep;  /* Per ms */
    float percentage;
    char fadeStop; // range 0-100
    FadeableBulb *next;
    static FadeableBulb *firstBulb;
    void fadeTick(double duration);


};

#endif // FADEABLEBULB_H
