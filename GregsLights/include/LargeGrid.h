#ifndef LARGEGRID_H
#define LARGEGRID_H

#include "controller/DDPOutput.h"
#include "GenericGrid.h"
#include "TimeInfo.h"
#include "Generator.h"
#define GRID_E11_COUNT 4*4
#define LGRID_PIXAL_HEIGHT 46
#define LGRID_PIXAL_WIDTH 106
#define LGRID_TOTAL_PIXALS LGRID_PIXAL_HEIGHT*LGRID_PIXAL_WIDTH

#define LGRID_DUMMY_HEIGHT LGRID_PIXAL_HEIGHT * 30
#define LGRID_DUMMY_WIDTH LGRID_PIXAL_WIDTH * 30


using namespace std;


class LargeGrid : public GenericGrid
{
public:
    LargeGrid(DDPOutput *net);
    virtual ~LargeGrid();
    virtual RGBLight * getBoard(int x, int y);
    virtual RGBLight * getPixal(int x, int y);
    void test();
    void run();
    char *getMessage();
protected:
private:
    char message[80];
    DDPOutput *outputNetwork;
    RGBLight *pixals[LGRID_TOTAL_PIXALS];
    RGBLight *board[ LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH];

};

#endif // LARGEGRID_H
