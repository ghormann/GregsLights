#ifndef COUNTDOWNCLOCK_H
#define COUNTDOWNCLOCK_H

#include "controller/Bulb.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>




#define CLOCK_DIGITS 7

class CountdownClock
{
    public:
        CountdownClock();
        virtual ~CountdownClock();
        void setBulb(int digit, int segment, Bulb *bulb);
        void setActive(bool);
        void tick();
        Bulb *getBulb(int digit, int segment);
        int getSecondsRemaining();
    protected:
    private:
        Bulb *bulbs[CLOCK_DIGITS*7 + 1];
        bool active;
        struct tm tm_christmas;
        time_t t_christmas;
        int lastTick;
        pthread_t clock_t;      /* Thread for writing to serial interface */

};

#endif // COUNTDOWNCLOCK_H
