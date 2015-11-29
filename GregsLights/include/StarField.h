#ifndef STARFIELD_H
#define STARFIELD_H

#include "controller/E131Network.h"
#include "TimeInfo.h"

#define STAR_COUNT 45

class StarField
{
    public:
        StarField(E131Network *net, TimeInfo *);
        virtual ~StarField();
        void test();
        RGBLight* getStar(int i);
        void setAll(RGBColor *c);
        void setAll(int r, int g, int b);
        void fadeAllTo(RGBColor *c, double duration);
        void fadeAllTo(int r, int g, int b, double duration);
        void setLeft(RGBColor *c);
        void setRight(RGBColor *c);
    protected:
    private:
        void swapStars(int i, int j);
        char message[80];
        TimeInfo *timeInfo;
    RGBLight *stars[STAR_COUNT];
    char startPos[STAR_COUNT];
};

#endif // STARFIELD_H
