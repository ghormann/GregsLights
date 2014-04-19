/*
 * A Single light that has both Red, Green, and Blue values that can be
 * mixed together to product any color
 */

#include "../include/RGBLight.h"

RGBLight::RGBLight(IPixal *red, IPixal *green, IPixal *blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}

void RGBLight::turnOff() {
    red->setIntensity(0);
    green->setIntensity(0);
    blue->setIntensity(0);
}

RGBLight::~RGBLight()
{
    //dtor
}
