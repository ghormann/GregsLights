#include "TimeInfo.h"
#include <stdio.h>
#include <ctype.h>


TimeInfo::TimeInfo(bool skip_time_check, bool show_new_year )
{
    //ctor
    this->skipTimeCheck = skip_time_check;
    this->newYears = show_new_year;
    this->num_seconds = 0;

    time(&t_now);
    tm_now = localtime(&t_now);
    tm_christmas.tm_sec    = 0;     //0
    tm_christmas.tm_min    = 0;     //0
    tm_christmas.tm_hour   = 0;     //0
    tm_christmas.tm_mday   = 25;    //25
    tm_christmas.tm_mon    = 11;    //11
    tm_christmas.tm_year   = tm_now->tm_year;
    tm_christmas.tm_isdst  = tm_now->tm_isdst;
    tm_christmas.tm_zone   = tm_now->tm_zone;
    tm_christmas.tm_gmtoff = tm_now->tm_gmtoff;

    t_christmas = mktime(&tm_christmas);

}

TimeInfo::~TimeInfo()
{
    //dtor
}

int TimeInfo::getNextYear()
{
    return tm_christmas.tm_year+1901;
}

void TimeInfo::tick()
{
    time(&t_now);
    tm_now = localtime(&t_now);
    num_seconds = difftime(t_christmas, t_now);
    num_seconds = (num_seconds < 0 ? 0 : num_seconds); // Don't go below zero
}

int TimeInfo::getHourOfDay()
{
    tick();
    return tm_now->tm_hour;
}

int TimeInfo::getSecondsUntil()
{
    tick();
    return num_seconds;
}

int TimeInfo::getHoursUntil()
{
    return getSecondsUntil()/3600;
}

bool TimeInfo::isDayLight()
{
    if (skipTimeCheck) return false;

    int hour = getHourOfDay();
    return (hour >=9 && hour < 17) ? true: false;
}

void TimeInfo::setSkipTimeCheck(bool skipIt)
{
    this->skipTimeCheck = skipIt;
}

bool TimeInfo::isDisplayHours()
{
    if (skipTimeCheck) return true;
    if (isDayLight()) return false;
    if (getSecondsUntil() < 60*60*4) return true; // all night long christmas, but not during the day.
    int hour = getHourOfDay();
    if (hour <6 || hour == 23) return false;

    return true;

}
