/*
 * A Single light that has both Red, Green, and Blue values that can be
 * mixed together to product any color
 */

#include "../../include/controller/RGBLight.h"

RGBLight::RGBLight(Bulb *red, Bulb *green, Bulb *blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}

void RGBLight::set(int red, int green, int blue)
{
    this->red->setIntensity(red);
    this->green->setIntensity(green);
    this->blue->setIntensity(blue);
}

void RGBLight::setStdColor(RGB_COLOR color)
{
    switch(color)
    {
    case RED:
        this->set(100,0,0);
        break;
    case GREEN:
        this->set(0,100,0);
        break;
    case BLUE:
        this->set(0,0,100);
        break;
    case YELLOW:
        this->set(100,100,0);
        break;
    case PURPLE:
        this->set(100,0,100);
        break;
    }
}

void RGBLight::turnOff()
{
    this->set(0,0,0);
}

RGBLight::~RGBLight()
{
    //dtor
}
