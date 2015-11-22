#ifndef RGBLIGHT_H
#define RGBLIGHT_H

#include "Bulb.h"
#include "RGBColor.h"

class RGBLight
{
public:
    RGBLight(Bulb *red, Bulb *green, Bulb *blue);
    virtual ~RGBLight();
    void turnOff();
    void copyFrom(RGBLight *other);
    void set(int red, int green, int blue);
    void set(RGBColor *color);
    void fade(int red_s, int green_s, int blue_s, int red_e, int green_e, int blue_e, double dur);
    void fadeTo(int red_e, int green_e, int blue_e, double dur);
    void fadeTo(RGBColor *color, double dur);
    int getRed();
    int getGreen();
    int getBlue();
    Bulb *getRedBulb();
    Bulb *getGreenBulb();
    Bulb *getBlueBulb();
protected:
private:
    Bulb *red;
    Bulb *green;
    Bulb *blue;
};

#endif // RGBLIGHT_H
