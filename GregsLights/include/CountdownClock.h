#ifndef COUNTDOWNCLOCK_H
#define COUNTDOWNCLOCK_H

#include "controller/E131Network.h"
#include "controller/Bulb.h"
#include "controller/RGBColor.h"
#include "TimeInfo.h"
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#define CLOCK_DIGITS    7
#define CLOCK_PIXALS_PER_DIGIT 43


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

enum clockUnits {SECONDS, MINUTES, HOURS, DAYS};

class CountdownClock
{
    public:
        CountdownClock(bool skip_time_check, bool show_new_year, E131Network *net[]);
        virtual ~CountdownClock();
        void setSpecial(int id, Bulb *blub);
        void setActive(bool);
        void setUnits(clockUnits u);
        void test();
        void testALlOn();
        void tick();
        void setAllOff();
        void setDigitColor(RGBColor *c);
        int getSecondsRemaining();
        char * getMessage();
        RGBLight * getPixel(int, int);
    protected:
    private:
        RGBLight *pixals[CLOCK_DIGITS * CLOCK_PIXALS_PER_DIGIT];
        void setDigit(int,int);
        RGBColor *color;
        Bulb *special[8];
        bool active;
        char message[100];
        int lastTick;
        clockUnits displayUnits;
        TimeInfo *timeinfo;
        bool reset;
        pthread_t clock_t;      /* Thread for writing to serial interface */

};

#endif // COUNTDOWNCLOCK_H
