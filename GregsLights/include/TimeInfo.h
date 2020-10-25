#ifndef TIMEINFO_H
#define TIMEINFO_H

#include <time.h>


class TimeInfo
{
public:
    static TimeInfo* getInstance();
    virtual ~TimeInfo();
    int getHourOfDay();
    int getSecondsOfDay();
    int getMinuteOfDay();
    int getSecondsUntil();
    int getHoursUntil();
    int getNextYear();
    bool isDayLight();
    bool isDisplayHours();
    bool isNoShow();
    bool isDebug();
    void setSkipTimeCheck(bool skipIt);
    bool isNewYears();
    bool isSkipTimeCheck();
    void setNoShow(bool b);
    void setNewYear(bool b);
    void setDebug(bool b);
protected:
    void tick();
private:
    TimeInfo(bool skipTimeCheck, bool newYears);
    void updateTmChristmas();
    static TimeInfo* m_pInstance;
    struct tm tm_christmas;
    time_t t_christmas;
    time_t t_now;
    struct tm *tm_now;
    int lastTick;
    bool skipTimeCheck;
    bool newYears;
    bool noShow;
    bool debug;
    int num_seconds;
};

#endif // TIMEINFO_H
