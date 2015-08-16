#ifndef LARGEGRID_H
#define LARGEGRID_H

#include "controller/E131Network.h"

#define GRID_E11_COUNT 29
#define GRID_PIXAL_HEIGHT 48
#define GRID_PIXAL_WIDTH 102
#define TOTAL_GRID_PIXALS GRID_PIXAL_HEIGHT*GRID_PIXAL_WIDTH


class LargeGrid
{
public:
    LargeGrid(E131Network *net[]);
    virtual ~LargeGrid();
    void test();
    void run();
    char *getMessage();
protected:
private:
    RGBLight *pixals[TOTAL_GRID_PIXALS];
    char message[80];

};

#endif // LARGEGRID_H
