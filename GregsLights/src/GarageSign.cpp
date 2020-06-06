#include "../include/GarageSign.h"
#include <sstream>
#include <iostream>

GarageSign::GarageSign(E131Network *net[], GregMQTT *mqtt) : GenericGrid(GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT,GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT), PowerCallbackInterface()
{
    this->mqtt = mqtt;
    mqtt->setPowerCallback(this);
    amps = 0;
    ampsChanged = true;

    int netId = 0;
    int cnt = 0;

    //TODO Change me to a real network
    for (int y =0; y < GARAGE_SIGN_HEIGHT; y++)
    {
        for (int i = 0; i < GARAGE_SIGN_WIDTH; i++)
        {
            //int x = GARAGE_SIGN_WIDTH - i - 1;
            int x = i;
            int pos = y * GARAGE_SIGN_WIDTH + x;

            //std::cout << " Garage (" << x << ", " << y << ") POS: " << pos << std::endl;


            pixals[pos] = new RGBLight(net[netId]->getBulb(cnt), net[netId]->getBulb(cnt+1), net[netId]->getBulb(cnt+2));
            //pixals[pos] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
            board[pos] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());

            cnt += 3;
            if (cnt >= 510) {
                ++netId;
                cnt = 0;
            }
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
        std::cout << "Illegal value of X for GarageSign getBoard" << std::endl;
        throw "Illegal value of X for GarageSign getBoard";
    }
    if (y >= GARAGE_SIGN_HEIGHT)
    {
        std::cout << "Illegal value of Y for GarageSign getBoard" << std::endl;
        throw "Illegal value of Y for GarageSign getBoard";
    }

    return board[(x*GARAGE_SIGN_HEIGHT) + y];

}
RGBLight *GarageSign::getPixal(int x, int y)
{
    if (x >= GARAGE_SIGN_WIDTH)
    {
        std::cout << "Illegal value of X for GarageSign getPixel" << std::endl;
        throw "Illegal value of X for GarageSign getPixel";
    }
    if (y >= GARAGE_SIGN_HEIGHT)
    {
        std::cout << "Illegal value of Y for GarageSign getPixel" << std::endl;
        throw "Illegal value of Y for GarageSign getPixel";
    }

    return pixals[(y*GARAGE_SIGN_WIDTH) + x];
}

void GarageSign::setPowerCallback(double amps) {
    this->amps = amps;
    this->ampsChanged = true;
}

void GarageSign::showPower() {
    int watts = amps * 115;
    std::ostringstream powerMsg;
    if (watts < 1000) {
        //powerMsg << "  ";
    } else if (watts < 10000) {
        powerMsg << " ";
    }
    powerMsg << watts;

    //setBackground(RGBColor::DARKGREEN, 0, 0, 160, GARAGE_SIGN_HEIGHT);
    //this->writeText(RGBColor::WHITE,2,2, "106.7FM",false);

    this->lockGrid();
    setBackground(RGBColor::RED, 0, 0,GARAGE_SIGN_WIDTH, GARAGE_SIGN_HEIGHT);
    this->writeText(RGBColor::WHITE,0,0,powerMsg.str(),false,2);
    this->releaseGrid();
    //this->writeText(RGBColor::WHITE,160,2,"WATTS",false);

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
    setDummyBackground(RGBColor::BLACK);
    this->writeText(RGBColor::WHITE,0,0, "66",false, 2);
/*
    getPixal(0,0)->set(RGBColor::GREEN);
    getPixal(GARAGE_SIGN_WIDTH-1,0)->set(RGBColor::PURPLE);
    getPixal(1,1)->set(RGBColor::BLUE);
    getPixal(0,1)->set(RGBColor::WHITE);
    getPixal(0,2)->set(RGBColor::WHITE);
    getPixal(0,3)->set(RGBColor::WHITE);
    getPixal(0,4)->set(RGBColor::WHITE);
    getPixal(0,5)->set(RGBColor::WHITE);
    getPixal(0,GARAGE_SIGN_HEIGHT - 1)->set(RGBColor::WHITE);
    getPixal(GARAGE_SIGN_WIDTH - 1,GARAGE_SIGN_HEIGHT - 1)->set(RGBColor::WHITE);
*/
    //showPower();
    sleep(2);

}
