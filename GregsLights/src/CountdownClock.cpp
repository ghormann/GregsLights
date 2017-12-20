#include "../include/CountdownClock.h"
#include <math.h>
#include <stdlib.h>

CountdownClock::CountdownClock(bool skip_time_check, bool show_new_year, E131Network *net[])
{
    this->active = false;
    this->reset = true;
    this->sparkel = false;
    this->clock_t = 0;
    this->lastTick = 0;
    this->color = RGBColor::WHITE;
    this->timeinfo = new TimeInfo(skip_time_check,show_new_year);
    this->displayUnits = SECONDS;

    int netId = 0;
    int cnt = 0;

    for (int i = 0 ; i < (CLOCK_DIGITS * CLOCK_PIXALS_PER_DIGIT);)
    {
        this->pixals[i] = net[netId]->getRGB(cnt*3);
        ++cnt;
        ++i;

        if (cnt==170)
        {
            cnt = 0;
            ++netId;
        }

    }
    sprintf(message, "Created Clock");

}

void CountdownClock::setUnits(clockUnits u)
{
    this->displayUnits = u;
    reset = true;
}

CountdownClock::~CountdownClock()
{
}

int CountdownClock::getSecondsRemaining()
{
    return lastTick;
}

void CountdownClock::testALlOn()
{
    while(1)
    {
        sprintf(message, "Setting all On");
        lastTick = 8888888;
        for (int i = 0; i < 7; i++)   // Last 7 digits
        {
            setDigit(i,8);
        }
        usleep(200 * 1000); // 200m
    }
}

void CountdownClock::test()
{
    sprintf(message, "Test() in clock: Looping though digits.");
    while(1)
    {
        this->setAllOff();
        printf(message, "Test() in clock: Special on");
        this->special[SPECIAL_STROBE]->setIntensity(100);
        usleep(200 * 1000*1000); // 2sec

        this->setAllOff();
        printf(message, "Test() in clock: Sent Sign on");
        this->special[SPECIAL_SIGN_ON]->setIntensity(100);
        usleep(200 * 1000*1000); // 2sec

        this->setAllOff();
        sprintf(message, "Test() in clock: Looping though digits.");
        for (int cnt = 0; cnt < 10; cnt++)
        {
            for (int i = 0; i< 7; i++)
            {
                setDigit(i,cnt);
                usleep(200 * 1000); // 200ms
            }
        }
    }
}

void CountdownClock::setAllOff()
{
    for (int i = 0 ; i < (CLOCK_DIGITS * CLOCK_PIXALS_PER_DIGIT); i++)
    {
        this->pixals[i]->set(0,0,0);
    }

    for (int i = 0; i <=6; i++) {
        this->special[i]->setIntensity(0);
    }
}


char * CountdownClock::getMessage()
{
    return message;
}

void CountdownClock::tick()
{
    char seconds_c[11];  /* Number of Seconds in Charactor */
    int num_seconds = timeinfo->getSecondsUntil();
    int num[10];


    // Not on douring the day
    if (!timeinfo->isDisplayHours())
    {
        sprintf(message, "Sleeping During Day: %d", timeinfo->getHourOfDay());
        setAllOff();
        lastTick = -1;
        sleep(5);
        return;
    }


    if (lastTick != num_seconds || reset == true)
    {
        reset = false;
        sprintf(message, "Running");
        if (num_seconds <= 0)
        {
            num_seconds = 0;
            this->sparkel = true;
            this->reset = true; /* Force faster update */
            this->setDigitColor(RGBColor::PURPLE);
            sprintf(seconds_c, "0000000");
            //special[SPECIAL_STROBE]->setIntensity(100);
        }
        else
        {
            double temp = num_seconds;
            switch(displayUnits)
            {
            case MINUTES:
                temp = temp/60;
                break;
            case HOURS:
                temp = temp/(3600);
                break;
            case DAYS:
                temp = (temp / 86400)+1;
                break;
            default:
                temp = temp;
                break;
            }
            sprintf(seconds_c, "%7d", ((int)round(temp)) );
        }

        for (int i = 0; i < 7; i++)   // Last 7 digits
        {
            if (isblank(seconds_c[i]))
            {
                num[i] = -1;
            }
            else
            {
                num[i] = seconds_c[i] - 48;
            }
            setDigit(i,num[i]);

        } // for i=1->7

        lastTick = num_seconds;
    }
}

RGBLight * CountdownClock::getPixel(int digit, int val)
{

    if (digit < 0 || digit >= CLOCK_DIGITS)
    {
        throw "invalid id for Digit";
    }
    if (val < 0 || val >= CLOCK_PIXALS_PER_DIGIT)
    {
        throw "invalid id for val";
    }

    return pixals[(digit * CLOCK_PIXALS_PER_DIGIT) + val];
}


void CountdownClock::setDigit(int digit, int val)
{
    digit = 6-digit;
    int data[CLOCK_PIXALS_PER_DIGIT];
    if (val < 0 || val > 9)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 0;
        }
    }
    else if (val ==0)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = (i < 23 && i >17) ? 0: 1;
        }
    }
    else if (val == 1)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 0;
            if (i >= 23 && i <= 30)
            {
                data[i] = 1;
            }
            if (i <= 5)
            {
                data[i] = 1;
            }
        }
    }
    else if (val == 2)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
            if ((i >23 && i<30) || (i>11 && i < 17))
            {
                data[i] = 0;
            }
        }
    }
    else if (val == 3)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
            if ((i >11 && i<18) || (i>36))
            {
                data[i] = 0;
            }
        }
    }
    else if (val == 4)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
            if (i > 5 && i < 11)
            {
                data[i] = 0;
            }
            if (i>30)
            {
                data[i] = 0;
            }
        }
    }
    else if (val == 5)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
            if (i <5 || i > 36)
            {
                data[i] = 0;
            }
        }
    }
    else if (val == 6)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
            if (i < 5)
            {
                data[i] = 0;
            }
        }
    }
    else if (val == 7)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 0;
            if ((i<12) || (i>22 && i<31))
            {
                data[i] = 1;
            }
        }
    }
    else if (val == 8)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
        }
    }
    else if (val == 9)
    {
        for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
        {
            data[i] = 1;
            if ((i>30) )
            {
                data[i] = 0;
            }
        }
    }

    // Set the data to the pixel
    for (int i = 0; i < CLOCK_PIXALS_PER_DIGIT; i++)
    {
        RGBColor *digitColor = RGBColor::BLACK;
        if (data[i] > 0)
        {
            digitColor = color; /* Current color */
            if (this->sparkel)
            {
                int random_number = rand() % 25;
                if (random_number == 0)
                {
                    digitColor = RGBColor::WHITE;
                }
            }
        }
        pixals[(digit * CLOCK_PIXALS_PER_DIGIT) + i]->set(digitColor);
    }
}

bool CountdownClock::isSparkel()
{
    return this->sparkel;
}

void CountdownClock::setDigitColor(RGBColor *c)
{
    this->color = c;
}

void * update_clock(void *args)
{
    CountdownClock *ptr = (CountdownClock *)args;
    while (1)
    {
        ptr->tick();
        /*
        Normally sleep 200ms, but if sparkel then sleep
        25 ms
        */
        usleep( (ptr->isSparkel() ? 25 : 200) * 1000); // 50-200ms
    }

    return NULL;
}


void CountdownClock::setSpecial(int id, Bulb *bulb)
{
    if (id < 0 || id >6)
    {
        throw "setSpecial: ID ust be between 0 and 6";
    }
    this->special[id] = bulb;
    bulb->setIntensity(0);
}


void CountdownClock::setActive(bool makeActive)
{

    if (makeActive)
    {
        if (! this->clock_t)
        {
            setAllOff();
            pthread_create(&(this->clock_t), NULL, update_clock, this);
        }
    }
    this->active = makeActive;
}

