#ifndef GENERICGRID_H
#define GENERICGRID_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MessageGenerator.h"
#include "TimeInfo.h"
#include "controller/RGBLight.h"

#include <cmath>

#define gjhDistance(dX0, dY0, dX1, dY1)  (sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0)))

enum GRID_SPECIAL
{
    GRID_TREE = 0,
    GRID_TREE_2,
    GRID_SNOWMEN,
    GRID_SNOWMEN_REVERSE,
    GRID_CANDY,
    GRID_DEER,
    GRID_DEER_2,
    GRID_DEER_2_RED,
    GRID_PRESENT_RED,
    GRID_PRESENT_GREEN,
    GRID_PRESENT_BLUE,
    GRID_PRESENT_PURPLE,
    GRID_SLEIGH
};


class GenericGrid
{
public:
    GenericGrid(int width_, int height_, int dummy_with_, int dummy_height, bool skipTime, bool newYears);
    virtual ~GenericGrid() = 0;
    virtual RGBLight * getBoard(int x, int y) = 0;
    virtual RGBLight * getPixal(int x, int y) = 0;
    void setDummyBackground(RGBColor *bgColor);
    void setDummyBackground(RGBColor *bgColor, int xStart, int yStart, int xEnd, int yEnd);
    void wipeToRight(RGBColor *color, double delay);
    void wipeDown(RGBColor *color, double delay);
    void setDisplayPosition(int x, int y);
    void redrawDisplay();
    int drawLetter(char letter, RGBColor *color, int x, int y);
    int drawLetterSmall(char letter, RGBColor *color, int x, int y);
    char *getMessage();
    void testGridLayout();
    void countdown();
    void drawSpecial(int x, int y, GRID_SPECIAL type);
    void scrollText(RGBColor *fgColor, RGBColor *bgColor, char * text, double speed);
protected:
    int height;
    int width ;
    int dummy_height;
    int currentX;
    int currentY;
    TimeInfo *timeInfo;
    int dummy_width;
    char message[80];
    MessageGenerator *generator;

private:
};

#endif // GENERICGRID_H
