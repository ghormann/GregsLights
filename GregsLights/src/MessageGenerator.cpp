#include "../include/MessageGenerator.h"

#include <stdlib.h>
#include <time.h>

MessageGenerator::MessageGenerator()
{
    //ctor
    srand (time(NULL));

    time_t t_now;
    time(&t_now);
    struct tm *tm_now = localtime(&t_now);

    isChristmas = (tm_now->tm_mday <= 25 ? true : false);
    clear();
}

MessageGenerator::~MessageGenerator()
{
    //dtor
}

void MessageGenerator::clear(){
    for (int i = 0; i < MESSAGE_GENERATOR_SIZE; i++)
    {
        bitmap[i] = 0;
    }

}

void MessageGenerator::checkClear() {
    for (int i = 0; i < MESSAGE_GENERATOR_SIZE; i++) {
        if (bitmap[i] == 0) return;
    }
    // If we got this far, everything is used up.  Clear it.
    clear();
}

char * MessageGenerator::getMessage()
{
    checkClear();

    if (isChristmas)
    {
        int i = 3;
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
            return "HE IS COMING.... ARE YOU READY?";
        case 2:
            return "ARE YOU READY FOR CHRISTMAS?      I BET THE KIDS ARE....";
        case 3:
            return "SECONDS UNTIL CHRISTMAS";
        case 4:
            return "^MERRY CHRISTMAS FROM THE HORMANN FAMILY ^";
        case 5:
            return "+ HAVE YOU BEEN NAUGHTY OR NICE? +";
        case 6:
            return "JOIN US AT MIDNIGHT TO SEE THE CLOCK HIT ZERO";


        }

    }
    else
    {

        return "Need to add some...";
    }

    return "SHOULD NOT GET HERE";
}
