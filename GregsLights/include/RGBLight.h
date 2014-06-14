#ifndef RGBLIGHT_H
#define RGBLIGHT_H

#include "../include/Bulb.h"

class RGBLight
{
    public:
        RGBLight(Bulb *red, Bulb *green, Bulb *blue);
        virtual ~RGBLight();
        void turnOff();
    void set(int red, int green, int blue);
protected:
    private:
        Bulb *red;
        Bulb *green;
        Bulb *blue;
};

#endif // RGBLIGHT_H
