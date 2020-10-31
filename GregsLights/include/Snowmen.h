#ifndef SNOWMEN_H
#define SNOWMEN_H

#include "pthread.h"
#include "controller/Bulb.h"
#include "controller/E131Network.h"
#include "TimeInfo.h"
#include "GenericGrid.h"
#include "GregMQTT.h"
#include "RGBPicture.h"

#define SNOWMEN_HEIGHT 95
#define SNOWMEN_WIDTH 47

#define SPLASH_GRID_HEIGHT 48
#define SPLASH_GRID_WIDTH 24

#define SKY_GRID_HEIGHT 24
#define SKY_GRID_WIDTH 96

#define SNOWMAN_RIGHT 1
#define SNOWMAN_LEFT  0

#define SNOWMAN_NOSE_HEIGHT (SNOWMEN_HEIGHT-65)

#define write_data(pause)  usleep((pause) * 1000000)

class SnowmanPicture
{
    public:
    RGBPicture *pic = NULL;
    int pic_offset_x = 0;
    int pic_offset_y = 0;
    std::string name = "Snowman";
    int mouth_offset_x = 0;
    int mouth_offset_y = 0;
    int splash_end_y = 0;
    int splash_offset_y = 0;
    int name_offset_x = 0;
    double mouth_multiplier = 1;
    bool show_misses = true;
};

class SnowmenGrid: public GenericGrid
{
public:
    SnowmenGrid(int width_, int height_, int dummy_with_, int dummy_height);
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
    Snowmen(bool skipTime, E131Network * network[], GregMQTT *mqtt);
    virtual ~Snowmen();
    void test_snowmen(void);
    void run();
    void setBulb(int i, Bulb *b);
    char * getMessage();
    char * getMessageWho();
    SnowmenGrid *getSnowmen(int pos);
    SnowmenGrid *getSplashGrid(int pos);
    SnowmenGrid *getSkyGrid();
    void lockSnowmen();
    void releaseSnowmen();
    pthread_mutex_t* getLock();
protected:
    void throwLeftStickNose(bool arch);
    void throwRightStickNose(bool arch);
    void throwHitHat(int pos);
    void throwGround(int pos, bool goHigh);
    void cannonShot(int snowmen_pos);
    void fadeSnow();
    void fadeNoseBalls();
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
    void drawSnowmen(int pos, bool withHat = true, int offset=0);
    void placeHatBack(int pos);
    void throwLeft(bool loft);
    void throwRight(bool loft);
    void hitNose(int snowmen, int start_y);
    void hitHat(int pos);
    void duckSnowman(int pos);
    void eatSnowball(int pos);
    void drawMouth(int pos, double mouthPos);
    void setAllOff();
    void publishMqtt();
    RGBColor *noseBallColor;
    pthread_mutex_t lock;
    double groundSnowLevel[2];
    int noseBalls[2];
    bool hatStatus[2];
    SnowmenGrid *snowmen[2];
    SnowmenGrid *splash[2];
    SnowmenGrid *skyGrid;
    void ball_line(GenericGrid *grid, int start_x, int start_y, int end_x, int end_y, double ballSize);
    void do_middle(int start_snowmen, int start_y, int high_y, int end_y,double duration);
    char message2[100];
    int snowmanStepCount = 0;
    char message_who[30];
    TimeInfo *timeinfo;
    SnowmanPicture *who_right;
    GregMQTT *mqtt;
    void createSnowmanPictures();
    std::vector<SnowmanPicture*> availSnowman;
};

#endif // SNOWMEN_H
