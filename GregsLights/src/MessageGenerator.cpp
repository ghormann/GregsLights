#include "../include/MessageGenerator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

std::string MessageGenerator::getStartDate() {
    std::string s = "OUR FULL DISPLAY STARTS NOV 23";
    return s;
}

/*
 * 1234567 -> "1,234,567"
 */
std::string MessageGenerator::formatNumber(int n)
{
    std::string digits = std::to_string(n);
    std::string result;
    int len = digits.size();
    for (int i = 0; i < len; i++)
    {
        if (i > 0 && (len - i) % 3 == 0)
        {
            result += ',';
        }
        result += digits[i];
    }
    return result;
}

/*
 * Time of day the clock will show the milestone ("8:14 PM")
 */
std::string MessageGenerator::getMilestoneTime(int milestone)
{
    char when[20];
    struct tm tm_when;
    time_t t = timeInfo->getTimeAtSecondsUntil(milestone);
    localtime_r(&t, &tm_when);
    strftime(when, sizeof(when), "%-I:%M %p", &tm_when);
    return std::string(when);
}

std::string MessageGenerator::getMilestoneAnnouncement(int milestone)
{
    std::string s = "AT ";
    s += getMilestoneTime(milestone);
    s += " WATCH THE CLOCK HIT ";
    s += formatNumber(milestone);
    s += "!";
    return s;
}

const char * MessageGenerator::getGreeting()
{
    return isChristmas ? "MERRY CHRISTMAS " : "HAPPY NEW YEAR ";
}

std::string MessageGenerator::getMessage()
{
    checkClear();

    int i = 11;
    while (bitmap[i] == 1)
    {
        i = rand() % MESSAGE_GENERATOR_SIZE;  // Don't show same message twice in a row.
    }
    bitmap[i] = 1;
    switch (i)
    {
    case 0:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        return "HOW MANY TIMES PER WEEK DO YOU CHECK THIS CLOCK?";
    case 1:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        if (isChristmas)
            return "HE IS COMING.... ARE YOU READY?";
        else
            return "WISHING YOU A GREAT NEW YEAR";
    case 2:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
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
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        if (isChristmas)
            return "+ HAVE YOU BEEN NAUGHTY OR NICE? +";
        else
            return "MADE YOUR RESOLUTIONS FOR THE NEW YEAR?";
    case 6:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
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
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        return "LISTEN TO THE LIGHTS AT 106.7 FM";

    case 11:
    case 12:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        return "TEXT YOUR FIRST NAME TO 888-887-1423 AND SEE YOUR NAME IN LIGHTS!";
    case 13:
    case 14:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        return "TEXT YOUR FIRST NAME TO 888-887-1423 FOR A PERSONALIZED MESSAGE.";
    case 15:
    {
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        const char * when = timeInfo->isNewYears() ? "THE NEW YEAR" : "CHRISTMAS";
        if (timeInfo->getHoursUntil() > 1)
            snprintf(message, sizeof(message), "\\ ONLY %d HOURS UNTIL %s \\", timeInfo->getHoursUntil(), when);
        else
            snprintf(message, sizeof(message), "\\ ONLY %d MINUTES UNTIL %s \\", timeInfo->getSecondsUntil()/60, when);
        return message;
    }
    case 16:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        if (!isChristmas)
            return "TIME TO START THOSE RESOLUTIONS!";
        if (timeInfo->getSecondsUntil() <= 0)
            return "HOPE SANTA WAS GOOD TO YOU!";
        if (timeInfo->getSleepsUntil() <= 1)
            return "SANTA IS LOADING HIS SLEIGH RIGHT NOW!";
        snprintf(message, sizeof(message), "ONLY %d MORE SLEEPS UNTIL CHRISTMAS!", timeInfo->getSleepsUntil());
        return message;
    case 17:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        if (!isChristmas)
            return "NEW YEAR, NEW LIGHTS... SAME CLOCK!";
        if (timeInfo->getHourOfDay() >= 21 || timeInfo->getHourOfDay() < 6)
            return "IT IS LATE... HE KNOWS WHEN YOU ARE SLEEPING!";
        return "SANTA IS CHECKING HIS LIST... TWICE!";
    case 18:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        return "WAVE AT THE SNOWMEN... THEY LOVE AN AUDIENCE!";
    case 19:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        if (!isChristmas)
        {
            snprintf(message, sizeof(message), "BRING ON %d!", timeInfo->getNextYear());
            return message;
        }
        if (timeInfo->getSleepsUntil() >= 2)
        {
            snprintf(message, sizeof(message), "RUDOLPH IS STRETCHING... TAKEOFF IN %d DAYS!", timeInfo->getSleepsUntil());
            return message;
        }
        return "RUDOLPH'S NOSE IS WARMED UP AND READY!";
    case 20:
        if (timeInfo->isNoShow()) {
            return getStartDate();
        }
        if (isChristmas)
            return "COOKIES FOR SANTA... CARROTS FOR THE REINDEER!";
        return "HAPPY HOLIDAYS FROM OUR HOUSE TO YOURS!";
    }

    return "SHOULD NOT GET HERE";
}
