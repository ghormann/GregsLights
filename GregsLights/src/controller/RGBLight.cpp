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

void RGBLight::copyFrom(RGBLight *other)
{
    this->set(other->getRed(), other->getGreen(), other->getBlue());
}

void RGBLight::fade(int red_s, int green_s, int blue_s, int red_e, int green_e, int blue_e, double dur)
{
    this->red->fade(red_s,red_e,dur);
    this->green->fade(green_s,green_e,dur);
    this->blue->fade(blue_s,blue_e,dur);
}

Bulb *RGBLight::getRedBulb() {
    return this->red;
}

Bulb *RGBLight::getGreenBulb() {
    return this->green;
}

Bulb *RGBLight::getBlueBulb() {
    return this->blue;
}

int RGBLight::getRed()
{
    return this->red->getIntensity();
}

int RGBLight::getGreen()
{
    return this->green->getIntensity();
}

int RGBLight::getBlue()
{
    return this->blue->getIntensity();
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
    case BLACK:
        this->set(0,0,0);
        break;
    case WHITE:
        this->set(100,100,100);
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
