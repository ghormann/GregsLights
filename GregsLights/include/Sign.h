#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"
#include "MessageGenerator.h"
#include "TimeInfo.h"
#include <cmath>

#define SIGN_OPTIONS 10
#define SIGN_E11_COUNT 12
#define SIGN_WIDTH 99  /* Was 48 */
#define SIGN_HEIGHT 20
#define TOTAL_SIGN_PIXALS SIGN_WIDTH * SIGN_HEIGHT

#define SIGN_DUMMY_HEIGHT SIGN_HEIGHT * 30
#define SIGN_DUMMY_WIDTH SIGN_WIDTH * 30

#undef TRUE
#define TRUE    1

#undef FALSE
#define FALSE   0

#define gjhDistance(dX0, dY0, dX1, dY1)  (sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0)))




enum SIGN_SPECIAL {
    SIGN_TREE = 0,
    SIGN_TREE_2,
    SIGN_SNOWMEN,
    SIGN_SNOWMEN_REVERSE,
    SIGN_CANDY,
    SIGN_DEER,
    SIGN_DEER_2,
    SIGN_DEER_2_RED,
    SIGN_PRESENT_RED,
    SIGN_PRESENT_GREEN,
    SIGN_PRESENT_BLUE,
    SIGN_PRESENT_PURPLE,
    SIGN_SLEIGH
};

class Sign
{
    public:
        Sign(bool skipTime, bool newYears, E131Network *net[]);
        virtual ~Sign();
        void test();
        void testGridLayout();
        void scrollText(RGBColor *fgColor, RGBColor *bgColor, char * text, double speed);
        void staticSecondsToGo(RGBColor *fgColor, RGBColor *bgColor);
        void flashSecondsToGo(int times, double delay);
        RGBLight * getPixal(int i);
        RGBLight * getPixal(int x, int y);
        void run();
        char *getMessage();
        void scrollSanta();
        void countdown();
        void colors();
    protected:
        RGBLight * getBoard(int x, int y);
        void setDisplayPosition(int x, int y);
        void redrawDisplay();
        int drawLetter(char letter, RGBColor *color, int x, int y);
        int drawLetterSmall(char letter, RGBColor *color, int x, int y);
        void setDummyBackground(RGBColor *bgColor);
        void setDummyBackground(RGBColor *bgColor, int xStart, int yStart, int xEnd, int yEnd);
        void drawSpecial(int x, int y, SIGN_SPECIAL type);
        void wipeToRight(RGBColor *color, double delay);
        void wipeDown(RGBColor *color, double delay);
        void rotateSecondsToGo();
        void fewTrees();
        void snowballFight();
        void clear();
        void checkClear();
    private:
        void moveBall(int x, RGBColor *bgColor, int snowballY);
        char useMap[SIGN_OPTIONS];
        int currentX;
        int currentY;
        TimeInfo *timeInfo;
        char message[80];
        E131Network *net1;
        RGBLight *pixals[TOTAL_SIGN_PIXALS];
        RGBLight *board[ SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH];
        MessageGenerator *generator;
};

#endif // SIGN_H
