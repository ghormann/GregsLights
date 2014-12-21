#include "../include/MessageGenerator.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

MessageGenerator::MessageGenerator()
{
    //ctor
    srand (time(NULL));

    time_t t_now;
    time(&t_now);
    struct tm *tm_now = localtime(&t_now);

    isChristmas = (tm_now->tm_mday <= 25 ? true : false);
    clear();
    timeInfo = new TimeInfo(true,true);
}

MessageGenerator::~MessageGenerator()
{
    //dtor
}

void MessageGenerator::clear()
{
    for (int i = 0; i < MESSAGE_GENERATOR_SIZE; i++)
    {
        bitmap[i] = 0;
    }

}

void MessageGenerator::checkClear()
{
    for (int i = 0; i < MESSAGE_GENERATOR_SIZE; i++)
    {
        if (bitmap[i] == 0) return;
    }
    // If we got this far, everything is used up.  Clear it.
    this->clear();
}

char * MessageGenerator::getMessage()
{
    checkClear();

    int i = 0;
    while (bitmap[i] == 1)
    {
        i = rand() % MESSAGE_GENERATOR_SIZE;  // Don't show same message twice in a row.
    }
    bitmap[i] = 1;
    switch (i)
    {
    case 0:
        return "HOW MANY TIMES PER DAY DO YOU CHECK THIS CLOCK?";
    case 1:
        if (isChristmas)
            return "HE IS COMING.... ARE YOU READY?";
        else
            return "WISHING YOU A GREAT NEW YEAR";
    case 2:
        if (isChristmas)
            if (timeInfo->getSecondsUntil() <= 0)
                return "MERRY CHRISTMAS";
            else
                return "ARE YOU READY FOR CHRISTMAS?      I BET THE KIDS ARE...." ;
        else
            return "HAPPY NEW YEAR";
    case 3:
        return "^ MERRY CHRISTMAS FROM THE HORMANN FAMILY ^";
    case 4:
        if (timeInfo->getSecondsUntil() > 0)
        {
            if (isChristmas)
                return "SECONDS UNTIL CHRISTMAS";
            else
            {
                sprintf(message, "SECONDS UNTIL %d", timeInfo->getNextYear());
                return message;
            }
        }

        return "THANKS FOR VISITING US";
    case 5:
        if (isChristmas)
            return "+ HAVE YOU BEEN NAUGHTY OR NICE? +";
        else
            return "MADE YOUR RESOLUTIONS FOR THE NEW YEAR?";
    case 6:
        if (timeInfo->getSecondsUntil() > 0)
            return "JOIN US TO SEE THE CLOCK HIT ZERO";
        else
            return "JOIN US NEXT YEAR TO SEE THE CLOCK HIT ZERO";
    case 7:
        if (timeInfo->getSecondsUntil() > 0)
        return "$ SECONDS LEFT #";
        else
        return "$ HAPPY HOLIDAYS #";
    case 8:
        if (timeInfo->getHoursUntil() > 1)
            sprintf(message, "\\ ONLY %d HOURS LEFT \\", timeInfo->getHoursUntil());
            else
            sprintf(message, "\\ ONLY %d MINUTES LEFT \\", timeInfo->getSecondsUntil()/60);
        return message;
    }

    return "SHOULD NOT GET HERE";
}
