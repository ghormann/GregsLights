#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"
#include "../include/CountdownClock.h"
#include "CountdownClock.h"
#include "MessageGenerator.h"
#include "TimeInfo.h"
#include "GenericGrid.h"
#include "GregMQTT.h"

#define SIGN_OPTIONS 14
#define SIGN_E11_COUNT 12
#define SIGN_WIDTH 96  /* Was 48 */
#define SIGN_HEIGHT 20
#define TOTAL_SIGN_PIXALS SIGN_WIDTH * SIGN_HEIGHT
#define SNOWBALL_OFFSET SIGN_WIDTH/4

#define SIGN_DUMMY_HEIGHT SIGN_HEIGHT * 30
#define SIGN_DUMMY_WIDTH SIGN_WIDTH * 30

#undef TRUE
#define TRUE    1

#undef FALSE
#define FALSE   0




class Sign: public GenericGrid
{
    public:
        Sign(CountdownClock *clock, bool skipTime, bool newYears, E131Network *net[], GregMQTT *mqtt);
        virtual ~Sign();
        void test();
        void testLines();
        void staticSecondsToGo(RGBColor *fgColor, RGBColor *bgColor);
        void flashSecondsToGo(int times, double delay);
        virtual RGBLight * getPixal(int x, int y);  /* Overriden */
        RGBLight * getPixal(int i);
        void run();
        void scrollSanta();
    protected:
        virtual RGBLight * getBoard(int x, int y);
        void rotateSecondsToGo();
        void fewTrees();
        void snowballFight();
        void clear();
        void checkClear();
        void radio();
        void sendStatus();
    private:
        void moveBall(int x, RGBColor *bgColor, int snowballY);
        void toGo(clockUnits);
        std::string wrapName(std::string name);
        CountdownClock *clock;
        GregMQTT *mqtt;
        char useMap[SIGN_OPTIONS];
        char message[80];
        E131Network *net1;
        RGBLight *pixals[TOTAL_SIGN_PIXALS];
        RGBLight *board[ SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH];
};

#endif // SIGN_H
