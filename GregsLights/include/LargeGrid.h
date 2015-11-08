#ifndef LARGEGRID_H
#define LARGEGRID_H

#include "controller/E131Network.h"
#include "GenericGrid.h"
#define GRID_E11_COUNT 4*4
#define LGRID_PIXAL_HEIGHT 46
#define LGRID_PIXAL_WIDTH 106
#define LGRID_TOTAL_PIXALS LGRID_PIXAL_HEIGHT*LGRID_PIXAL_WIDTH

#define LGRID_DUMMY_HEIGHT LGRID_PIXAL_HEIGHT * 30
#define LGRID_DUMMY_WIDTH LGRID_PIXAL_WIDTH * 30



class LargeGrid : public GenericGrid
{
public:
    LargeGrid(bool skipTime, bool newYears, E131Network *net[], E131Network *net2[]);
    virtual ~LargeGrid();
    virtual RGBLight * getBoard(int x, int y);
    virtual RGBLight * getPixal(int x, int y);

    void test();
    void run();
    char *getMessage();
protected:
private:
    RGBLight *pixals[LGRID_TOTAL_PIXALS];
    RGBLight *board[ LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH];

    char message[80];

};

#endif // LARGEGRID_H
