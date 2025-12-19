#ifndef GARAGESIGN_H_INCLUDED
#define GARAGESIGN_H_INCLUDED

#include "controller/DummyBulb.h"
#include "controller/RGBLight.h"
#include "controller/DDPOutput.h"
#include "TimeInfo.h"
#include "GenericGrid.h"
#include "GregMQTT.h"
#define GARAGE_SIGN_WIDTH ((32*11))
#define GARAGE_SIGN_HEIGHT ((16*3))
#define GARAGE_PIXELS ((GARAGE_SIGN_WIDTH) * (GARAGE_SIGN_HEIGHT))

class GarageSign: public GenericGrid, public PowerCallbackInterface
{
public:
    GarageSign(DDPOutput *net, GregMQTT *mqtt);
    virtual ~GarageSign();
    void test();
    void run();
    virtual RGBLight * getPixal(int x, int y);  /* Overriden */
    virtual RGBLight * getBoard(int x, int y); /* Overridden */
    virtual void setPowerCallback(double power); /* Overridden */
    virtual void setCurrentPowerCallback(double dollars, double kwh);


private:
    void showPower();
    void showPopcorn();
    void showTextNumber();
    void showPowerToday();
    void showPowerTodayVideo();
    void drawRadio(int radio_left);
    GregMQTT *mqtt;
    void showStartDate();
    double amps;
    bool ampsChanged;
    double dollars;
    double kwh;
    RGBLight *pixals[GARAGE_SIGN_WIDTH * GARAGE_SIGN_HEIGHT];
    RGBLight *board[ GARAGE_SIGN_WIDTH * GARAGE_SIGN_HEIGHT];

};




#endif // GARAGESIGN_H_INCLUDED
