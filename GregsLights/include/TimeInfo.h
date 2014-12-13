#ifndef TIMEINFO_H
#define TIMEINFO_H

#include <time.h>


class TimeInfo
{
public:
    TimeInfo(bool skipTimeCheck, bool newYears);
    virtual ~TimeInfo();
    int getHourOfDay();
    int getSecondsUntil();
    int getHoursUntil();
    bool isDayLight();
    bool isDisplayHours();
protected:
    void tick();
private:
    struct tm tm_christmas;
    time_t t_christmas;
    time_t t_now;
    struct tm *tm_now;
    int lastTick;
    bool skipTimeCheck;
    bool newYears;
    int num_seconds;
};

#endif // TIMEINFO_H
