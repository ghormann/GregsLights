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

void RGBLight::set(RGBColor *color)
{
    this->red->setIntensity(color->getRed());
    this->green->setIntensity(color->getGreen());
    this->blue->setIntensity(color->getBlue());
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

// Fade from current to....
void RGBLight::fadeTo(int red_e, int green_e, int blue_e, double dur)
{
    this->fade(getRed(), getGreen(), getBlue(), red_e, green_e, blue_e, dur);
}

void RGBLight::fadeTo(RGBColor *color, double dur)
{
    this->fade(getRed(),getGreen(),getBlue(), color->getRed(),color->getGreen(),color->getBlue(), dur);
}

Bulb *RGBLight::getRedBulb()
{
    return this->red;
}

Bulb *RGBLight::getGreenBulb()
{
    return this->green;
}

Bulb *RGBLight::getBlueBulb()
{
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


void RGBLight::turnOff()
{
    this->set(0,0,0);
}

RGBLight::~RGBLight()
{
    //dtor
}
