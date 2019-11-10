#ifndef TIMEINFO_H
#define TIMEINFO_H

#include <time.h>


class TimeInfo
{
public:
    static TimeInfo* getInstance();
    virtual ~TimeInfo();
    int getHourOfDay();
    int getSecondsUntil();
    int getHoursUntil();
    int getNextYear();
    bool isDayLight();
    bool isDisplayHours();
    bool isNoShow();
    void setSkipTimeCheck(bool skipIt);
    bool isNewYears();
    void setNoShow(bool b);
    void setNewYear(bool b);
protected:
    void tick();
private:
    TimeInfo(bool skipTimeCheck, bool newYears);
    static TimeInfo* m_pInstance;
    struct tm tm_christmas;
    time_t t_christmas;
    time_t t_now;
    struct tm *tm_now;
    int lastTick;
    bool skipTimeCheck;
    bool newYears;
    bool noShow;
    int num_seconds;
};

#endif // TIMEINFO_H
