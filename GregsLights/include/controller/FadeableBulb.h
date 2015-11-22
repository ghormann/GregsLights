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
    void setDebug(bool d);
protected:
private:
    static FadeableBulb *firstBulb;
    void fadeTick(double duration);
    FadeableBulb *next = 0;
    double fadeStep = 0;  /* Per ms */
    double percentage = 0;
    char fadeStop = 0; // range 0-100
    bool debug = false;


};

#endif // FADEABLEBULB_H
