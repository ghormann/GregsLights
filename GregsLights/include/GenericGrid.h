#ifndef GENERICGRID_H
#define GENERICGRID_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MessageGenerator.h"
#include "TimeInfo.h"
#include "controller/RGBLight.h"
#include "RGBPicture.h"

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
    void setBackground(RGBColor *bgColor);
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
    void writeText(RGBColor *fgColor, int x, int y, std::string text);
    void writeTextSmall(RGBColor *fgColor, int x, int y, std::string text);
    void scrollText(RGBColor *fgColor, RGBColor *bgColor, char * text, double speed);
    void candyCane();
    void colorAroundPicture(RGBPicture *pict, int duration);
    void scrollPictureLeft(RGBPicture &pic, double speed, bool hideBlack);
    void scrollPictureRight(RGBPicture &pic, double speed, bool hideBlack);
    void scrollPictureUp(RGBPicture &pic, double speed, bool hideBlack);
    void showPictureNow(RGBPicture &pic, int posX, int posY,bool hideBlack);
    void showPictureDummy(RGBPicture &pic, int posX, int posY,bool hideBlack);
    void spiral(RGBColor *color);
    void showMovie(std::string &startsWith, int cnt, double duration, int x, int y);
    void showMovieCenter(std::string &startsWith, int cnt, double duration);
    void interruptThread();
protected:
    int gridHeight;
    int gridWidth ;
    int dummy_height;
    int dummy_width;
    int currentX;
    int currentY;
    TimeInfo *timeInfo;
    char message[80];
    MessageGenerator *generator;
    int _gridSleep(double d);
    bool interrupt;

private:
};

#endif // GENERICGRID_H
