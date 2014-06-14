#include "../include/Bush.h"

Bush::Bush(int id, Bulb *r, Bulb *g, Bulb *b, Bulb *w)
{
    this->bushId = id;
    this->red = r;
    this->green = g;
    this->blue = b;
    this->white = w;
}

int Bush::getId()
{
    return bushId;
}

void Bush::setAll(int pct)
{
    red->setIntensity(pct);
    green->setIntensity(pct);
    blue->setIntensity(pct);
    white->setIntensity(pct);
}

Bush::~Bush()
{
    //dtor
}
