#include "../../include/controller/RGBColor.h"

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

int RGBColor::getRed() {
    return red;
}

int RGBColor::getGreen() {
    return green;
}

int RGBColor:: getBlue()
{
    return blue;
}

RGBColor *RGBColor::RED = new RGBColor(100,0,0);
RGBColor* RGBColor::GREEN = new RGBColor(0,100,0);
RGBColor* RGBColor::BLUE = new RGBColor(0,0,100);
RGBColor* RGBColor::BLACK = new RGBColor(0,0,0);
RGBColor* RGBColor::PURPLE = new RGBColor(100,0,100);
RGBColor* RGBColor::YELLOW = new RGBColor(100,100,0);
RGBColor* RGBColor::WHITE = new RGBColor(100,100,100);

