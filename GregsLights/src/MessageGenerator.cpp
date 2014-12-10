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
    lastId = -1;

}

MessageGenerator::~MessageGenerator()
{
    //dtor
}

char * MessageGenerator::getMessage()
{
    if (isChristmas)
    {
        int i = lastId;
        while (i == lastId)
        {
            i = rand() % 6;  // Don't show same message twice in a row.
        }
        lastId = i;   //
        switch (i)
        {
        case 0:
            return "HOW MANY TIMES PER DAY DO YOU CHECK THIS CLOCK?";
        case 1:
            return "HE IS COMING.... ARE YOU READY?";
        case 2:
            return "ARE YOU READY FOR CHRISTMAS?          I BET THE KIDS ARE....";
        case 3:
            return "SECONDS UNTIL CHRISTMAS";
        case 4:
            return "^MERRY CHRISTMAS FROM THE HORMANN FAMILY ^";
        case 5:
            return "HAVE YOU BEEN NAUGHTY OR NICE?";

        }

    }
    else
    {

        return "Need to add some...";
    }

    return "SHOULD NOT GET HERE";
}
