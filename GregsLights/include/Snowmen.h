#ifndef SNOWMEN_H
#define SNOWMEN_H

#include "pthread.h"
#include "controller/Bulb.h"
#include "TimeInfo.h"
#include "GenericGrid.h"

#define SNOWMEN_HEIGHT 95
#define SNOWMEN_WIDTH 47

#define SPLASH_GRID_HEIGHT 48
#define SPLASH_GRID_WIDTH 24

#define SKY_GRID_HEIGHT 24
#define SKY_GRID_WIDTH 95

#define SNOWMAN_RIGHT 1
#define SNOWMAN_LEFT  0

#define SNOWMAN_NOSE_HEIGHT (SNOWMEN_HEIGHT-65)


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
    SnowmenGrid *getSplashGrid(int pos);
    SnowmenGrid *getSkyGrid();
    void lockSnowmen();
    void releaseSnowmen();
protected:
    void throwLeftStickNose(bool arch);
    void throwRightStickNose(bool arch);
    void throwHitHat(int pos);
    void throwGround(int pos, bool goHigh);
    void fadeSnow();
    void drawGroundSnow(int pos, RGBColor* color);
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
    void drawSnowmen(int pos, bool withHat = true);
    void placeHatBack(int pos);
    void throwLeft(bool loft);
    void throwRight(bool loft);
    void hitNose(int snowmen, int start_y);
    void hitHat(int pos);
    pthread_mutex_t lock;
    pthread_t snowman_t;
    double groundSnowLevel[2];
    bool hatStatus[2];
    SnowmenGrid *snowmen[2];
    SnowmenGrid *splash[2];
    SnowmenGrid *skyGrid;
    void ball_line(GenericGrid *grid, int start_x, int start_y, int end_x, int end_y, double ballSize);
    void do_middle(int start_snowmen, int start_y, int high_y, int end_y,double duration);
    char message2[100];
    TimeInfo *timeinfo;
};

#endif // SNOWMEN_H
