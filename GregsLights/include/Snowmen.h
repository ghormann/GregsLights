#ifndef SNOWMEN_H
#define SNOWMEN_H

#include "pthread.h"
#include "controller/Bulb.h"
#include "TimeInfo.h"
#include "GenericGrid.h"
#define SNOWMEN_HEIGHT 95
#define SNOWMEN_WIDTH 47

#define write_data(pause)  usleep((pause) * 1000000)

class SnowmenGrid: public GenericGrid
{
public:
    SnowmenGrid(int width_, int height_, int dummy_with_, int dummy_height, bool skipTime, bool newYears);
    virtual RGBLight * getPixal(int x, int y);  /* Overriden */
    void setPixal(int x, int y, RGBLight * p);
protected:
    virtual RGBLight * getBoard(int x, int y);
private:
    int getPos(int x, int y);
    RGBLight *pixals[SNOWMEN_HEIGHT * SNOWMEN_WIDTH];
    RGBLight *board[SNOWMEN_HEIGHT * SNOWMEN_WIDTH];

};


class Snowmen
{
public:
    Snowmen(bool skipTime);
    virtual ~Snowmen();
    void test_snowmen(void);
    void run();
    void setBulb(int i, Bulb *b);
    char * getMessage();
    SnowmenGrid *getSnowmen(int pos);
protected:
    /*
        void hit_low_right(double pause);
        void hit_low_left(double pause);
        void hit_high_left(double pause);
        void hit_high_right(double pause);
        void miss_going_left(double pause);
        void miss_going_right(double pause);
        void catch_going_left(double pause, double pause2);
        void catch_going_right(double pause, double pause2);
        void set_snowmen(int side, int,int,int,int,int,int,int);
        */
    void do_it_snowmen();
private:
    pthread_t snowman_t;
    SnowmenGrid *snowmen[2];
    char message2[100];
    TimeInfo *timeinfo;
};

#endif // SNOWMEN_H
