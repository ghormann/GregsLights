#ifndef LARGEGRID_H
#define LARGEGRID_H

#include "controller/E131Network.h"

#define GRID_E11_COUNT 1
#define TOTAL_GRID_PIXALS 100


class LargeGrid
{
public:
    LargeGrid(E131Network *net[]);
    virtual ~LargeGrid();
    void test();
    void run();

protected:
private:
    RGBLight *pixals[TOTAL_GRID_PIXALS];

};

#endif // LARGEGRID_H
