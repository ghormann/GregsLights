#include "../include/GarageSign.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

GarageSign::GarageSign(DDPOutput *net, GregMQTT *mqtt) : GenericGrid(GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT,GARAGE_SIGN_WIDTH,GARAGE_SIGN_HEIGHT), PowerCallbackInterface()
{
    this->mqtt = mqtt;
    mqtt->setPowerCallback(this);
    amps = 0;
    ampsChanged = true;
    this->generator = new MessageGenerator(TimeInfo::getInstance());


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


            pixals[pos] = new RGBLight(net->getBulb(cnt), net->getBulb(cnt+1), net->getBulb(cnt+2));
            //pixals[pos] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
            board[pos] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());

            cnt += 3;
        }
    }
}

GarageSign::~GarageSign()
{
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

void GarageSign::setCurrentPowerCallback(double dollars, double kwh)
{
    this->kwh = kwh;
    this->dollars = dollars;
    this->ampsChanged = true;
}

void GarageSign::setPowerCallback(double amps)
{
    this->amps = amps;
    this->ampsChanged = true;
}

void GarageSign::showPower()
{
    int watts = amps * 115;
    std::string FM_STATION = "106.7FM";
    std::string units = "Watts";
    std::string RADIO = "RADIO STATION";
    std::string footer = "TO POWER THE LIGHTS";

    std::ostringstream powerMsg;

    if (watts < 1000)
    {
        powerMsg << "  ";
    }
    else if (watts < 10000)
    {
        powerMsg << " ";
    }
    powerMsg << watts;

    int radio_left = 0;
    int power_left = 134;

    // Switch sides every hour
    if ((TimeInfo::getInstance()->getHourOfDay() %2) == 0)
    {
        radio_left = 220;
        power_left = 0;
    }

    std::string msg = powerMsg.str();
    sprintf(message, "%s Watts", msg.c_str());

    this->lockGrid();
    this->setBackground(RGBColor::BLACK);
    setBackground(RGBColor::DARKGREEN, radio_left, 0, radio_left+128, GARAGE_SIGN_HEIGHT);
    this->writeTextNew(RGBColor::WHITE,radio_left,2, FM_STATION,false, 32);
    this->writeTextNew(RGBColor::WHITE,radio_left,32, RADIO,false, 16);

    setBackground(RGBColor::DARKRED, power_left, 0,power_left+216, GARAGE_SIGN_HEIGHT);
    this->writeTextNew(RGBColor::WHITE,power_left+14,2,msg,false,32);
    this->writeTextNew(RGBColor::WHITE,power_left+111,2,units,false, 32);
    this->writeTextNew(RGBColor::WHITE,power_left+13,32, footer,false, 16);

    this->releaseGrid();

}

void GarageSign::showPowerToday()
{
    int power_left = 134;
    double duration = 0.06;
    int cnt = 0;
    int max = 174;
    std::string startsWith = "clark_Plug";
    std::vector<RGBPicture *> pics;

    RGBPicture::findStartsWith(startsWith, pics);
    std::vector<RGBPicture *>::iterator it;

    setBackground(RGBColor::BLACK);

    it = pics.begin();

    // Ever other time we want to show the back half of the moveie
    if ((timeInfo->getMinuteOfDay() % 2)  == 0)
    {
        max = 900;
        // Run off some pictures
        for (int i = 0; i < 175; i++)
        {
            if (it != pics.end())
            {
                it++;
            }
        }
    }

    for(; it != pics.end(); it++)
    {
        // decided not to display all images
        if (++cnt > max)
        {
            break;
        }
        RGBPicture * p = (*it);
        std::ostringstream todayMsg;
        std::ostringstream line2;
        todayMsg << "$" << std::setprecision(2);
        todayMsg << std::fixed << this->dollars;
        todayMsg << " Spent on";
        std::string m = todayMsg.str();
        this->lockGrid();

        this->showPictureNow(*p,20,0,false);
        setBackground(RGBColor::DARKRED, power_left, 0,power_left+216, GARAGE_SIGN_HEIGHT);
        this->writeTextNew(RGBColor::WHITE,power_left+14,2,m,false,24);

        if (kwh < 100)
        {
            line2 << std::setprecision(2) << std::fixed  << kwh;
        }
        else
        {
            line2 << std::setprecision(1) << std::fixed  << kwh;
        }
        line2 << " KWh Today";
        m = line2.str();
        this->writeTextNew(RGBColor::WHITE,power_left+14,26,m,false,24);
        this->releaseGrid();
        sprintf(message, "%s %s", todayMsg.str().c_str(), m.c_str());

        gridSleep(duration);
    }
}


void GarageSign::run()
{
    TimeInfo *timeInfo = TimeInfo::getInstance();

    if (timeInfo->isDebug())
    {
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

    if (TimeInfo::getInstance()->isNoShow())
    {
        showStartDate();
    }
    else
    {

        // Wait in a tight loop to update if
        // signalled.   Does waste a little CPU.
        for (int i = 0; i < 1000; i++)
        {
            if (ampsChanged)
            {
                if (timeInfo->getSecondsOfDay() > 47)
                {
                    showPowerToday();
                }
                showPower();
                ampsChanged = false;
                break;
            }
            gjhSleep(0.1);
        }

    }
}

void GarageSign::showStartDate()
{
    std::string s = generator->getStartDate();
    this->writeTextNew(RGBColor::getRandom(),0,2, s,false,20);
    gjhSleep(3.0);

    int i = rand() % 3;
    switch(i)
    {
    case 0:
        wipeDown(RGBColor::BLACK, 0.1);
        break;
    case 1:
        wipeToRight(RGBColor::BLACK, 0.005);
        gjhSleep(1.0);
        break;
    case 2:
        gjhSleep(1.0);
        break;
    }
}

void GarageSign::test()
{
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
