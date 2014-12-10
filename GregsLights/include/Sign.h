#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"
#include "MessageGenerator.h"

#define SIGN_WIDTH 48
#define SIGN_HEIGHT 20
#define TOTAL_SIGN_PIXALS SIGN_WIDTH * SIGN_HEIGHT

#define SIGN_DUMMY_HEIGHT 50
#define SIGN_DUMMY_WIDTH 1000

#undef TRUE
#define TRUE    1

#undef FALSE
#define FALSE   0


enum SIGN_SPECIAL {
    SIGN_TREE = 0,
    SIGN_SNOWMEN,
    SIGN_CANDY
};

class Sign
{
    public:
        Sign(bool skipTime, E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4, E131Network *n5, E131Network *n6);
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
    protected:
        RGBLight * getBoard(int x, int y);
        void setDisplayPosition(int x, int y);
        void redrawDisplay();
        int drawLetter(char letter, RGBColor *color, int x, int y);
        int drawLetterSmall(char letter, RGBColor *color, int x, int y);
        void setDummyBackground(RGBColor *bgColor);
        void drawSpecial(int x, int y, SIGN_SPECIAL type);
        void wipeToRight(RGBColor *color, double delay);
        void wipeDown(RGBColor *color, double delay);
        void rotateSecondsToGo();
        void fewTrees();
    private:
        int currentX;
        int currentY;
        bool skipTimeCheck;
        char message[80];
        E131Network *net1;
        RGBLight *pixals[TOTAL_SIGN_PIXALS];
        RGBLight *board[ SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH];
        MessageGenerator *generator;
};

#endif // SIGN_H
