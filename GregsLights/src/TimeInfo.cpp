#include "TimeInfo.h"
#include <stdio.h>
#include <ctype.h>
#include <math.h>

TimeInfo* TimeInfo::m_pInstance = NULL;

TimeInfo* TimeInfo::getInstance()
{
    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = new TimeInfo(false,false);

    return m_pInstance;
}


TimeInfo::TimeInfo(bool skip_time_check, bool show_new_year )
{
    //ctor
    this->skipTimeCheck = skip_time_check;
    this->newYears = show_new_year;
    this->num_seconds = 0;
    this->noShow = false;
    this->debug = false;

    time(&t_now);
    tm_now = localtime(&t_now);
    updateTmChristmas();

}

TimeInfo::~TimeInfo()
{
    //dtor
}

void TimeInfo::updateTmChristmas()
{
    tm_christmas.tm_sec    = 0;     //0
    tm_christmas.tm_min    = 0;     //0
    tm_christmas.tm_hour   = 0;     //0
    tm_christmas.tm_mday   = 25;    //25
    tm_christmas.tm_mon    = 11;    //11
    tm_christmas.tm_year   = tm_now->tm_year;
    tm_christmas.tm_isdst  = tm_now->tm_isdst;
    tm_christmas.tm_zone   = tm_now->tm_zone;
    tm_christmas.tm_gmtoff = tm_now->tm_gmtoff;

    if (this->isNewYears())
    {
        tm_christmas.tm_sec    = 59;
        tm_christmas.tm_min    = 59;
        tm_christmas.tm_hour   = 23;
        tm_christmas.tm_mday   = 31;
        tm_christmas.tm_mon    = 11;
    }

    t_christmas = mktime(&tm_christmas);
    /* Debug */
    //t_christmas = t_now + 60;

}

int TimeInfo::getNextYear()
{
    return tm_christmas.tm_year+1901;
}

void TimeInfo::setNewYear(bool b)
{
    this->newYears = b;
    this->updateTmChristmas();
}

bool TimeInfo::isNewYears()
{
    return newYears;
}

bool TimeInfo::isNoShow()
{
    return noShow;
}

void TimeInfo::setNoShow(bool b)
{
    this->noShow = b;
}

bool TimeInfo::isDebug()
{
    return debug;
}

void TimeInfo::setDebug(bool b)
{
    this->debug = b;
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
    time(&t_now);
    return tm_now->tm_hour;
}

int TimeInfo::getSecondsOfDay()
{
    time(&t_now);
    return tm_now->tm_sec;
}

int TimeInfo::getMinuteOfDay()
{
    time(&t_now);
    return tm_now->tm_min;
}


int TimeInfo::getSecondsUntil()
{
    tick();
    return num_seconds;
}

int TimeInfo::getHoursUntil()
{
    // tick() done by getSecodnsUntil()
    double d = getSecondsUntil();
    d = d/3600;
    return (int) round(d);
}

bool TimeInfo::isDayLight()
{
    if (skipTimeCheck)
        return false;

    int hour = getHourOfDay();
    return (hour >=9 && hour < 17) ? true: false;
}

void TimeInfo::setSkipTimeCheck(bool skipIt)
{
    this->skipTimeCheck = skipIt;
}

bool TimeInfo::isSkipTimeCheck()
{
    return this->skipTimeCheck;
}

bool TimeInfo::isDisplayHours()
{
    if (skipTimeCheck)
        return true;
    if (isDayLight())
        return false;
    if (getSecondsUntil() < 60*60*4)
        return true; // all night long christmas, but not during the day.
    int hour = getHourOfDay();
    if (hour <6 || hour == 23)
        return false;

    return true;

}
