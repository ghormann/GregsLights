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

void RGBLight::set(int red, int green, int blue) {
    this->red->setIntensity(red);
    this->green->setIntensity(green);
    this->blue->setIntensity(blue);
}


void RGBLight::turnOff() {
    this->set(0,0,0);
}

RGBLight::~RGBLight()
{
    //dtor
}
