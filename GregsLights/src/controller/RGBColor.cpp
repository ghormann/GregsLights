#include "../../include/controller/RGBColor.h"
#include <stdlib.h>

RGBColor::RGBColor(int r, int g, int b)
{
    //ctor
    this->red =r;
    this->green = g;
    this->blue = b;
}

RGBColor::~RGBColor()
{
    //dtor
}

int RGBColor::getRed()
{
    return red;
}

int RGBColor::getGreen()
{
    return green;
}

int RGBColor:: getBlue()
{
    return blue;
}

RGBColor *RGBColor::getRandom()
{
    int i = rand() % 7;
    switch(i)
    {
    case 0:
        return RED;
    case 1:
        return GREEN;
    case 2:
        return BLUE;
    case 3:
        return PURPLE;
    case 4:
        return YELLOW;
    case 5:
        return WHITE;
    case 6:
        return ORANGE;
    }

    return WHITE;
}

RGBColor *RGBColor::RED = new RGBColor(100,0,0);
RGBColor* RGBColor::GREEN = new RGBColor(0,100,0);
RGBColor* RGBColor::BLUE = new RGBColor(0,0,100);
RGBColor* RGBColor::BLACK = new RGBColor(0,0,0);
RGBColor* RGBColor::PURPLE = new RGBColor(100,0,100);
RGBColor* RGBColor::YELLOW = new RGBColor(100,100,0);
RGBColor* RGBColor::WHITE = new RGBColor(100,100,100);
RGBColor* RGBColor::UNKNOWN = new RGBColor(0,100,100);
RGBColor* RGBColor::ORANGE = new RGBColor(100,50,0);
RGBColor* RGBColor::GREY = new RGBColor(25,25,25);
RGBColor* RGBColor::BROWN = new RGBColor(50,20,0);
RGBColor* RGBColor::LIGHTBROWN = new RGBColor(81,56,0);

