#include "../include/CountdownClock.h"

CountdownClock::CountdownClock(bool skip_time_check, bool show_new_year)
{
    this->active = false;
    this->clock_t = 0;
    this->lastTick = 0;
    this->timeinfo = new TimeInfo(skip_time_check,show_new_year);

    for (int i = 0 ; i < CLOCK_DIGITS * 7; i++)
    {
        this->bulbs[i] = 0;
    }
    sprintf(message, "Created Clock");

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
    sprintf(message, "All on");
    while(1)
    {
        for (int i =0; i <6; i++)
        {
            for (int j = 0; j<7; j++)
            {
                getBulb(i,j)->setIntensity(100);

            }
        }
        //getBulb(5,5)->setIntensity(100);
        for (int i = 0; i < 6; i++)
        {
            special[i]->setIntensity(100);
        }
        //sleep(1);
        usleep(200 * 1000); // 200m
    }
}

void CountdownClock::test()
{
    sprintf(message, "Testing Clock");
    while(0) {
	testALlOn();
	}

    while(1)
    {

        for (int i = 0; i < 3; i++)
        {
            special[SPECIAL_STROBE]->setIntensity(100);
            sleep(1);
            special[SPECIAL_STROBE]->setIntensity(0);
            sleep(1);
        }

        special[SPECIAL_HORZ]->setIntensity(100);
        sleep(1);
        special[SPECIAL_HORZ]->setIntensity(0);

        special[SPECIAL_P3]->setIntensity(100);
        sleep(1);
        special[SPECIAL_P3]->setIntensity(0);

        special[SPECIAL_P5]->setIntensity(100);
        sleep(1);
        special[SPECIAL_P5]->setIntensity(0);

        special[SPECIAL_P6]->setIntensity(100);
        sleep(1);
        special[SPECIAL_P6]->setIntensity(0);

        for (int i = 0; i < 6; i++)   //0 to < 6
        {
            for (int j = 0; j < 7; j++)
            {
                getBulb(i,j)->setIntensity(100);
                lastTick = i * 100 + j;
                sleep(1);
                getBulb(i,j)->setIntensity(0);
            }
        }
    }
}

void CountdownClock::setAllOff()
{
    for (int i = 0; i < 6; i++)   //0 to < 6
    {
        for (int j = 0; j < 7; j++)
        {
            getBulb(i,j)->setIntensity(0);
        }
    }
    special[SPECIAL_HORZ]->setIntensity(0);
    special[SPECIAL_P3]->setIntensity(0);
    special[SPECIAL_P5]->setIntensity(0);
    special[SPECIAL_P6]->setIntensity(0);

}

void CountdownClock::setFirst(int value)
{

    if (value == 3)
    {
        special[SPECIAL_HORZ]->setIntensity(100);
        special[SPECIAL_P3]->setIntensity(100);
        special[SPECIAL_P5]->setIntensity(0);
        special[SPECIAL_P6]->setIntensity(100);
    }
    else if (value == 2)
    {
        special[SPECIAL_HORZ]->setIntensity(100);
        special[SPECIAL_P3]->setIntensity(100);
        special[SPECIAL_P5]->setIntensity(100);
        special[SPECIAL_P6]->setIntensity(0);
    }
    else if (value == 1)
    {
        special[SPECIAL_HORZ]->setIntensity(0);
        special[SPECIAL_P3]->setIntensity(100);
        special[SPECIAL_P5]->setIntensity(0);
        special[SPECIAL_P6]->setIntensity(100);
    }
    else
    {
        special[SPECIAL_HORZ]->setIntensity(0);
        special[SPECIAL_P3]->setIntensity(0);
        special[SPECIAL_P5]->setIntensity(0);
        special[SPECIAL_P6]->setIntensity(0);
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
    int s1 = 0;
    int s2 = 0;
    int s3 = 0;
    int s4 = 0;
    int s5 = 0;
    int s6 = 0;
    int s7 = 0;


    // Not on douring the day
    if (timeinfo->isDayLight())
    {
        sprintf(message, "Sleeping During Day: %d", timeinfo->getHourOfDay());
        setAllOff();
        lastTick = -1;
        sleep(5);
        return;
    }


    if (lastTick != num_seconds)
    {
        sprintf(message, "Running");
        if (num_seconds <= 0)
        {
            num_seconds = 0;
            sprintf(seconds_c, "0000000");
            special[SPECIAL_STROBE]->setIntensity(100);
        }
        else
        {
            sprintf(seconds_c, "%7d", num_seconds);
        }

        // set tehe first Digit (Special)
        if (isblank(seconds_c[0]))
        {
            setFirst(0);
        }
        else
        {
            num[0] = seconds_c[0] - 48;
            setFirst(num[0]);
        }

        // The real digits are identifeid as 0-6, but they are the
        // last digits, so we need to off set by one here when refereincing
        // seconds_c

        for (int i = 0; i < 6; i++)   // Last 6 digits
        {
            if (isblank(seconds_c[i+1]))
            {
                num[i] = -1;
            }
            else
            {
                num[i] = seconds_c[i+1] - 48;
            }

            s1=s2=s3=s4=s5=s6=s7=0;
            if (num[i] == 0)
            {
                s1=s3=s6=s7=s5=s2 = 100;
            }
            else if (num[i] == 1)
            {
                s3=s6=100;
            }
            else if (num[i] == 2)
            {
                s1=s3=s4=s5=s7=100;
            }
            else if (num[i] == 3)
            {
                s1=s3=s4=s6=s7=100;
            }
            else if (num[i] == 4)
            {
                s2=s4=s3=s6=100;
            }
            else if (num[i] == 5)
            {
                s1=s2=s4=s6=s7=100;
            }
            else if (num[i] == 6)
            {
                s1=s2=s5=s4=s6=s7=100;
            }
            else if (num[i] == 7)
            {
                s1=s3=s6=100;
            }
            else if (num[i] == 8)
            {
                s1=s2=s3=s4=s5=s6=s7=100;
            }
            else if (num[i] == 9)
            {
                s1=s2=s3=s4=s6=100;
            }

            getBulb(i,0)->setIntensity(s1);
            getBulb(i,1)->setIntensity(s2);
            getBulb(i,2)->setIntensity(s3);
            getBulb(i,3)->setIntensity(s4);
            getBulb(i,4)->setIntensity(s5);
            getBulb(i,5)->setIntensity(s6);
            getBulb(i,6)->setIntensity(s7);

        } // for i=1->7

        //Digit zero is special

        // Need to add code here.

        lastTick = num_seconds;
    }

}

void * update_clock(void *args)
{
    CountdownClock *ptr = (CountdownClock *)args;
    while (1)
    {
        ptr->tick();
        usleep(200 * 1000); // 200m
    }

    return NULL;
}

Bulb *CountdownClock::getBulb(int digit, int segment)
{
    if (digit < 0 || digit > CLOCK_DIGITS -1 )
    {
        throw "Invalid number of Digits (Get)";
    }

    if (segment < 0 || segment > 6 )
    {
        throw "Invalid number of Segments (Get)";
    }

    int pos = (digit * 7) + segment;
    return this->bulbs[pos];

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

    int pos = (digit * 7) + segment;
    this->bulbs[pos] = bulb;
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

