#ifndef GREGSDISPLAY_H
#define GREGSDISPLAY_H

#include "DisplayModel.h"
#include "TimeInfo.h"

class GregsDisplay
{
public:
    GregsDisplay(DisplayModel *model);
    virtual ~GregsDisplay();
    void run();
    DisplayModel *getModel();
protected:
private:
    DisplayModel *model;
    TimeInfo *timeinfo;
    static void * dmxThread(void *);
    static void * bushThread(void *);
    static void * signThread(void *);
    static void * snowmenThread(void *);
    static void * gridThread(void *);
    static void * starThread(void *);
    void do_it_bushes();
    void set_bush(int bush, int color, int intensity);
    void fade_bush(int bush, int color, int s_intensity, int f_intensity, double duration);
    void setAllOff();
    void rotate_some();
    void fadeThroughAll(double f_duration, double h_duration);
    void fadeAllHouse(int color, int s_intensity, int f_intensity, double duration);
    void setAllToColor(int color, int intensity);
    void setAllHouse(int color, int intensity);
    void fadeAllBush(int color, int s_intensity, int f_intensity, double duration);
    void test_bushes();
    void change_one(void);
    void slide_down(void);
    void moveFasterRight();
    void moveFasterLeft();
    void moveFromMiddle();
    void redGreenFade();
    void fadeWhite();
    void fade_offset();
    void set_house(int light, int color, int intensity);
    void fade_house(int light, int color, int s_intensity, int f_intensity, double duration);
    void rotate_all(double duration, int cycles);
    void chase_left(int baseColor, int diffColor, double startDuration,
                    double decreasePct, double stopDuration);
    void chase_right(int baseColor, int diffColor, double startDuration,
                     double decreasePct, double stopDuration);


};

#endif // GREGSDISPLAY_H
