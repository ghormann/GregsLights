#ifndef SNOWMEN_H
#define SNOWMEN_H

#include "pthread.h"
#include "controller/Bulb.h"
#include "TimeInfo.h"
#define SNOWMEN_MAX_BULBS 18

#define write_data(pause)  usleep((pause) * 1000000)


class Snowmen
{
public:
    Snowmen(bool skipTime);
    virtual ~Snowmen();
    void test_snowmen(void);
    void run();
    void setBulb(int i, Bulb *b);
    char * getMessage();
protected:
    void hit_low_right(double pause);
    void hit_low_left(double pause);
    void hit_high_left(double pause);
    void hit_high_right(double pause);
    void miss_going_left(double pause);
    void miss_going_right(double pause);
    void catch_going_left(double pause, double pause2);
    void catch_going_right(double pause, double pause2);;
    void do_it_snowmen();
    void set_snowmen(int side, int,int,int,int,int,int,int);
private:
    pthread_t snowman_t;
    char message2[100];
    Bulb *bulbs[SNOWMEN_MAX_BULBS];
    void setBodies(bool on);
    TimeInfo *timeinfo;
};

#endif // SNOWMEN_H
