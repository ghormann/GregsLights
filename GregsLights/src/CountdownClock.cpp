#include "../include/CountdownClock.h"

CountdownClock::CountdownClock()
{
    this->active = false;
    this->clock_t = 0;

    for (int i = 0 ; i < CLOCK_DIGITS * 7; i++)
    {
        this->bulbs[i] = 0;
    }

    //ctor
}

CountdownClock::~CountdownClock()
{
    //dtor
}

void * update_clock(void *args)
{
    //NetworkCollection *ptr = (NetworkCollection *)args;
    while (1)
    {
        //ptr->doUpdate();
        usleep(200 * 1000); // 200ms
    }

    return NULL;
}


void CountdownClock::setBulb(int digit, int segment, Bulb *bulb)
{
    if (digit < 0 || digit > CLOCK_DIGITS -1 )
    {
        throw "Invalid number of Digits";
    }

    if (segment < 0 || segment > 6 )
    {
        throw "Invalid number of Segments";
    }

    int pos = digit * CLOCK_DIGITS + segment;
    this->bulbs[pos] = bulb;
}

void CountdownClock::setActive(bool makeActive)
{

    if (makeActive)
    {
        for (int i = 0 ; i < CLOCK_DIGITS * 7; i++)
        {
            if (this->bulbs[i] == 0)
                throw "CountdownClock: Not all of the bulbs have been initialized";
            // Start thread
            if (! this->clock_t)
                pthread_create(&(this->clock_t), NULL, update_clock, NULL);
        }
    }

    this->active = makeActive;
}

