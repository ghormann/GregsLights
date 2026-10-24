#ifndef TIMEINFO_H
#define TIMEINFO_H

#include <time.h>

/* How long the clock, sign and garage celebrate after hitting a milestone */
#define MILESTONE_CELEBRATE_SECONDS 15


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
    int getSleepsUntil();
    static bool isMilestone(int seconds);
    int getNextMilestone(int withinSeconds);
    int getRecentMilestone(int secondsUntil, int withinSeconds);
    time_t getTimeAtSecondsUntil(int seconds);
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
