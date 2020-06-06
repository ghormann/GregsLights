#ifndef GARAGESIGN_H_INCLUDED
#define GARAGESIGN_H_INCLUDED

#include "controller/DummyBulb.h"
#include "controller/RGBLight.h"
#include "controller/E131Network.h"
#include "TimeInfo.h"
#include "GenericGrid.h"
#include "GregMQTT.h"
#define GARAGE_SIGN_WIDTH 96
#define GARAGE_SIGN_HEIGHT 32
#define GARAGE_E131_COUNT 19

class GarageSign: public GenericGrid, public PowerCallbackInterface
{
public:
    GarageSign(E131Network *net[], GregMQTT *mqtt);
    virtual ~GarageSign();
    void test();
    void run();
    virtual RGBLight * getPixal(int x, int y);  /* Overriden */
    virtual RGBLight * getBoard(int x, int y); /* Overridden */
    virtual void setPowerCallback(double power); /* Overridden */


private:
    void showPower();
    GregMQTT *mqtt;
    double amps;
    bool ampsChanged;
    char message[80];
    RGBLight *pixals[GARAGE_SIGN_WIDTH * GARAGE_SIGN_HEIGHT];
    RGBLight *board[ GARAGE_SIGN_WIDTH * GARAGE_SIGN_HEIGHT];

};




#endif // GARAGESIGN_H_INCLUDED
