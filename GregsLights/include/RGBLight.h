#ifndef RGBLIGHT_H
#define RGBLIGHT_H

#include "../include/Bulb.h"

enum RGB_COLOR{
  RED = 0,
  GREEN,
  BLUE,
  YELLOW,
  PURPLE
};

class RGBLight
{
    public:
        RGBLight(Bulb *red, Bulb *green, Bulb *blue);
        virtual ~RGBLight();
        void turnOff();
    void set(int red, int green, int blue);
    void setStdColor(RGB_COLOR color);
protected:
    private:
        Bulb *red;
        Bulb *green;
        Bulb *blue;
};

#endif // RGBLIGHT_H
