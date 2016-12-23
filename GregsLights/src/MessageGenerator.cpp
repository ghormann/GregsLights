#include "../include/MessageGenerator.h"

#include <stdlib.h>
#include <stdio.h>

MessageGenerator::MessageGenerator(TimeInfo *ti)
{
    //ctor
    timeInfo = ti;
    isChristmas = ! ti->isNewYears();
    clear();
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

    int i = 8;
    while (bitmap[i] == 1)
    {
        i = rand() % MESSAGE_GENERATOR_SIZE;  // Don't show same message twice in a row.
    }
    bitmap[i] = 1;
    switch (i)
    {
    case 0:
        return "HOW MANY TIMES PER WEEK DO YOU CHECK THIS CLOCK?";
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
            if (timeInfo->getSecondsUntil() <= 0)
                return "HARD TO BELIEVE ANOTHER YEAR HAS PASSED";
            else
                return "SO FEW SECONDS LEFT THIS YEAR";
    case 3:
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
            if (isChristmas)
                return "JOIN US TO SEE THE CLOCK HIT ZERO CHRISTMAS EVE";
            else
                return "SECONDS LEFT THIS YEAR";
        else
            return "JOIN US NEXT YEAR TO SEE THE CLOCK HIT ZERO";
    case 7:
    case 8:
    case 9:
        if (timeInfo->getSecondsUntil() > 0)
            if (isChristmas)
                return "$ SECONDS UNTIL CHRISTMAS #";
            else
                return "$ SECONDS UNTIL THE NEW YEAR #";
        else
            return "$ HAPPY HOLIDAYS #";
    case 10:
        return "LISTEN TO THE LIGHTS AT 106.7 FM";

    case 11:
        const char * when = timeInfo->isNewYears() ? "THE NEW YEAR" : "CHRISTMAS";
        if (timeInfo->getHoursUntil() > 1)
            sprintf(message, "\\ ONLY %d HOURS UNTIL %s \\", timeInfo->getHoursUntil(), when);
        else
            sprintf(message, "\\ ONLY %d MINUTES UNTIL %s \\", timeInfo->getSecondsUntil()/60, when);
        return message;

    }

    return "SHOULD NOT GET HERE";
}
