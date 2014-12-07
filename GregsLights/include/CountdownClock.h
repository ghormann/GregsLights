#ifndef COUNTDOWNCLOCK_H
#define COUNTDOWNCLOCK_H

#include "controller/Bulb.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>


#define CLOCK_DIGITS    6


        //Special
        //0: Strobe
        //1: Not Used
        //2: POS 6
        //3: Horizontal
        //4: POS 3
        //5: POS 4
#define SPECIAL_STROBE  0
#define SPECIAL_P6      2
#define SPECIAL_HORZ    3
#define SPECIAL_P3      4
#define SPECIAL_P5      5

class CountdownClock
{
    public:
        CountdownClock(bool skip_time_check, bool show_new_year);
        virtual ~CountdownClock();
        void setBulb(int digit, int segment, Bulb *bulb);
        void setSpecial(int id, Bulb *blub);
        void setActive(bool);
        void test();
        void testALlOn();
        void tick();
        void setAllOff();
        Bulb *getBulb(int digit, int segment);
        int getSecondsRemaining();
        void setFirst(int value);
        char * getMessage();
    protected:
    private:
        Bulb *bulbs[CLOCK_DIGITS*7 + 1];
        Bulb *special[8];
        bool active;
        char message[100];
        struct tm tm_christmas;
        time_t t_christmas;
        int lastTick;
        bool skipTimeCheck;
        bool newYears;
        pthread_t clock_t;      /* Thread for writing to serial interface */

};

#endif // COUNTDOWNCLOCK_H
