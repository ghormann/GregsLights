#include "../include/GarageSign.h"
#include <sstream>

GarageSign::GarageSign(E131Network *net[], GregMQTT *mqtt) : GenericGrid(GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT,GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT), PowerCallbackInterface()
{
    this->mqtt = mqtt;
    mqtt->setPowerCallback(this);
    amps = 0;
    ampsChanged = true;

    //TODO Change me to a real network
    for (int i =0; i < GARAGE_SIGN_WIDTH; i++)
    {
        for (int j = 0; j < GARAGE_SIGN_HEIGHT; j++)
        {
            int pos = i * GARAGE_SIGN_HEIGHT + j;
            pixals[pos] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
            board[pos] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
        }
    }
}

GarageSign::~GarageSign() {
// Todo
}


RGBLight *GarageSign::getBoard(int x, int y)
{
    if (x >= GARAGE_SIGN_WIDTH)
    {
        throw "Illegal value of X for GarageSign getBoard";
    }
    if (y >= GARAGE_SIGN_HEIGHT)
    {
        throw "Illegal value of X for GarageSign getBoard";
    }

    return board[(x*GARAGE_SIGN_HEIGHT) + y];

}
RGBLight *GarageSign::getPixal(int x, int y)
{
    if (x >= GARAGE_SIGN_WIDTH)
    {
        throw "Illegal value of X for GarageSign getBoard";
    }
    if (y >= GARAGE_SIGN_HEIGHT)
    {
        throw "Illegal value of X for GarageSign getBoard";
    }

    return pixals[(x*GARAGE_SIGN_HEIGHT) + y];
}

void GarageSign::setPowerCallback(double amps) {
    this->amps = amps;
    this->ampsChanged = true;
}

void GarageSign::showPower() {
    int watts = amps * 115;
    std::ostringstream powerMsg;
    powerMsg << "POWER: " << watts << " WATTS";

    setBackground(RGBColor::DARKGREEN, 0, 0, 160, GARAGE_SIGN_HEIGHT);
    this->writeText(RGBColor::WHITE,2,2, "106.7 FM",false);

    setBackground(RGBColor::RED, 160, 0,GARAGE_SIGN_WIDTH, GARAGE_SIGN_HEIGHT);
    this->writeText(RGBColor::WHITE,160,2,powerMsg.str(),false);

}

void GarageSign::run() {

    TimeInfo *timeInfo = TimeInfo::getInstance();

    if (timeInfo->isDebug()) {
        test();
        return;
    }

        // Not on douring the day
    if (!timeInfo->isDisplayHours())
    {
        sprintf(message, "Sleeping During the day (%02d)",
                timeInfo->getHourOfDay());
        setDummyBackground(RGBColor::BLACK);
        setDisplayPosition(0,0);
        sleep(2);
        return;
    }

    // Wait in a tight loop to update if
    // signalled.   Needs changed to a ThreadWakeup.
    for (int i = 0; i < 1000;i++) {
        if (ampsChanged) {
            showPower();
            ampsChanged = false;
        }
        gjhSleep(0.1);
    }
}

void GarageSign::test() {
    showPower();

}
