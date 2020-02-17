#include "../include/GarageSign.h"

GarageSign::GarageSign(E131Network *net[], GregMQTT *mqtt) : GenericGrid(GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT,GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT)
{
    this->mqtt = mqtt;

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
        throw "Illegae value of X for GarageSign getBoard";
    }
    if (y >= GARAGE_SIGN_HEIGHT)
    {
        throw "Illegae value of X for GarageSign getBoard";
    }

    return board[(x*GARAGE_SIGN_HEIGHT) + y];

}
RGBLight *GarageSign::getPixal(int x, int y)
{
    if (x >= GARAGE_SIGN_WIDTH)
    {
        throw "Illegae value of X for GarageSign getBoard";
    }
    if (y >= GARAGE_SIGN_HEIGHT)
    {
        throw "Illegae value of X for GarageSign getBoard";
    }

    return pixals[(x*GARAGE_SIGN_HEIGHT) + y];
}

void GarageSign::showPower() {
    setBackground(RGBColor::BLUE);
    gjhSleep(2.0);

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

    showPower();
}

void GarageSign::test() {
    showPower();

}
