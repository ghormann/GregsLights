#ifndef RGBLIGHT_H
#define RGBLIGHT_H

#include "Bulb.h"

enum RGB_COLOR
{
    RED = 0,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    BLACK,
    WHITE
};

class RGBLight
{
public:
    RGBLight(Bulb *red, Bulb *green, Bulb *blue);
    virtual ~RGBLight();
    void turnOff();
    void copyFrom(RGBLight *other);
    void set(int red, int green, int blue);
    void fade(int red_s, int green_s, int blue_s, int red_e, int green_e, int blue_e, double dur);
    void setStdColor(RGB_COLOR color);
    int getRed();
    int getGreen();
    int getBlue();
protected:
private:
    Bulb *red;
    Bulb *green;
    Bulb *blue;
};

#endif // RGBLIGHT_H
