#ifndef RGBLIGHT_H
#define RGBLIGHT_H

#include "../include/IPixal.h"

class RGBLight
{
    public:
        RGBLight(IPixal *red, IPixal *green, IPixal *blue);
        virtual ~RGBLight();
        void turnOff();
    protected:
    private:
        IPixal *red;
        IPixal *green;
        IPixal *blue;
};

#endif // RGBLIGHT_H
