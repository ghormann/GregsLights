#ifndef STARFIELD_H
#define STARFIELD_H

#include "controller/E131Network.h"

#define STAR_COUNT 45

class StarField
{
    public:
        StarField(E131Network *net);
        virtual ~StarField();
        void test();
        RGBLight* getStar(int i);
    protected:
    private:
        void swapStars(int i, int j);
    RGBLight *stars[STAR_COUNT];
};

#endif // STARFIELD_H
