#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"

#define SIGN_WIDTH 48
#define SIGN_HEIGHT 20
#define TOTAL_SIGN_PIXALS SIGN_WIDTH * SIGN_HEIGHT

#define SIGN_DUMMY_HEIGHT 50
#define SIGN_DUMMY_WIDTH 1000

class Sign
{
    public:
        Sign(E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4, E131Network *n5, E131Network *n6);
        virtual ~Sign();
        void test();
        void scrollSecondsUntil(RGBColor *fgColor, RGBColor *bgColor);
        void staticSecondsUntil(RGBColor *fgColor, RGBColor *bgColor);
        RGBLight * getPixal(int i);
        RGBLight * getPixal(int x, int y);
    protected:
        RGBLight * getBoard(int x, int y);
        void setDisplayPosition(int x, int y);
        void redrawDisplay();
        int drawLetter(char letter, RGBColor *color, int x, int y);
        int drawLetterSmall(char letter, RGBColor *color, int x, int y);
    private:
        int currentX;
        int currentY;
        E131Network *net1;
        RGBLight *pixals[TOTAL_SIGN_PIXALS];
        RGBLight *board[ SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH];
};

#endif // SIGN_H
