#ifndef LARGEGRID_H
#define LARGEGRID_H

#include "controller/E131Network.h"
#include "GenericGrid.h"
#include "TimeInfo.h"
#define GRID_E11_COUNT 4*4
#define LGRID_PIXAL_HEIGHT 46
#define LGRID_PIXAL_WIDTH 106
#define LGRID_TOTAL_PIXALS LGRID_PIXAL_HEIGHT*LGRID_PIXAL_WIDTH

#define LGRID_DUMMY_HEIGHT LGRID_PIXAL_HEIGHT * 30
#define LGRID_DUMMY_WIDTH LGRID_PIXAL_WIDTH * 30

enum GRID_ACTIONS
{
    LG_SLEEP = 0,
    LG_GRINCH,
    LG_TRAIN_HORMANN,
    LG_HAT,
    LG_SHOW_PICT
};


using namespace std;


class LargeGrid : public GenericGrid
{
public:
    LargeGrid(bool skipTime, bool newYears, E131Network *net[], E131Network *net2[]);
    virtual ~LargeGrid();
    virtual RGBLight * getBoard(int x, int y);
    virtual RGBLight * getPixal(int x, int y);
    void scrollGrinch();
    void test();
    void run();
    void peakSanta();
    void trainText(string left);
    char *getMessage();
    void setNextAction(GRID_ACTIONS a);
protected:
private:
    RGBLight *pixals[LGRID_TOTAL_PIXALS];
    RGBLight *board[ LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH];
    TimeInfo *timeinfo;
    GRID_ACTIONS nextAction;

    char message[80];

};

#endif // LARGEGRID_H
