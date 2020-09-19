#include "../include/DisplayModel.h"
#include "../include/controller/RGBLight.h"
#include "../include/controller/OpenDMXNetwork.h"
#include "../include/controller/LORNetwork.h"
#include "../include/controller/E131Network.h"
#include "../include/controller/NetworkCollection.h"
#include "../include/controller/Bush.h"
#include "../include/Sign.h"
#include "../include/GarageSign.h"
#include <string.h>
#include <iostream>


DisplayModel::DisplayModel(bool sendDMX, int skip_time_check, int show_new_year)
{
    std::string clientId ="clock_" + std::to_string(time(NULL));
    this->mqtt = new GregMQTT(true /*sendDMX*/, clientId.c_str());
    int signId = 0;
    this->skipTimeCheck = (skip_time_check == TRUE? true : false);
    this->newYears = (show_new_year == TRUE ? true: false);
    /* initialize random seed: */
    srand (time(NULL));

    TimeInfo::getInstance()->setNewYear(this->newYears);
    TimeInfo::getInstance()->setSkipTimeCheck(this->skipTimeCheck);

    //Initialize Memory Buffers
    for (int i = 0; i < NUM_MESSAGE_BUFFERS; i++)
    {
        messages[i] = new char[MAX_MESSAGE_LENGTH+1];
        messages[i][0] = 0;
    }

    /*
     * msBetween: Millisonds between checking for updates (Tick)
     * maxTicks: Max number of updates skipped before force
     * maxBeforeSleep: Max Universes updated before taking an extra sleep
     * extraSleepMs: Duration (MS) of sleep when MaxBeforeSleep hit.
     */
    networks = new NetworkCollection("Normal", 50,1,60,1, false);
    // name, 5, 2, 6, 15 is "OK" for Alpha Pix.
    // name: 5,2,6,5 I Fairly good.
    networkClock = new NetworkCollection("Clock", 5,2,6,1, true);
    networkSnowman = new NetworkCollection("Snowmen", 50,1,99,1,false);
    networkP10 = new NetworkCollection("P10", 50,1,99,1,false);


    //LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB0", sendDMX);
    //LORNetwork *lor = new LORNetwork((char*) "/dev/ttyS0", sendDMX);


    char *signIP = "192.168.1.232";
    E131Network *sign[SIGN_E11_COUNT];
    sign[signId++] = new E131Network(signIP, 40, 512);  // port 4 170
    sign[signId++] = new E131Network(signIP, 41, 512);  // port 4 170
    sign[signId++] = new E131Network(signIP, 42, 512);  // port 4 170
    sign[signId++] = new E131Network(signIP, 43, 512);  // port 4 130
    sign[signId++] = new E131Network(signIP, 10, 512);  // port 1 170
    sign[signId++] = new E131Network(signIP, 11, 512);  // port 1 170
    sign[signId++] = new E131Network(signIP, 12, 512);  // port 1 170
    sign[signId++] = new E131Network(signIP, 13, 512);  // port 1 130
    sign[signId++] = new E131Network(signIP, 20, 512);  // port 2 170
    sign[signId++] = new E131Network(signIP, 21, 512);  // port 2 170
    sign[signId++] = new E131Network(signIP, 22, 512);  // port 2 170
    sign[signId++] = new E131Network(signIP, 23, 512);  // port 2 130

    // port 3 is used by the clock
    E131Network *clockNetwork[2];
    clockNetwork[0] = new E131Network(signIP, 30, 512);  // port 2 130
    clockNetwork[1] = new E131Network(signIP, 31, 512);  // port 2 130

    // P10
    E131Network *p10[GARAGE_E131_COUNT];
    for (int i = 0; i < GARAGE_E131_COUNT; i++) {
        //std::cout << "Creating P10: " << i << std::endl;
        p10[i] = new E131Network("192.168.1.147", i+1, 512);
        if (sendDMX) {
            networkP10->addNetwork(p10[i]);
        }
    }

    //DMX port
    E131Network *clockE131 = new E131Network(signIP, 100, 512); // Universe 100 of Sign Controller

    if (sendDMX)
    {
        networks->addNetwork(clockE131);
        //networks->addNetwork(lor);
        networks->addNetwork(clockNetwork[0]);
        networks->addNetwork(clockNetwork[1]);
        for (int j = 0; j< SIGN_E11_COUNT; j++)
        {
            networkClock->addNetwork(sign[j]);
        }

    }

    //setup Clock
    this->clock = new CountdownClock(this->skipTimeCheck, this->newYears, clockNetwork, this->getMqtt());

    for (int i=0; i <=6; i++)
    {
        this->clock->setSpecial(i, clockE131->getBulb(i+21));
    }

    this->sign = new Sign(clock, sign, this->getMqtt());


    char *snowmanIP = "192.168.1.239";
    E131Network *snowmanList[99];
    for (int i = 0; i < 99; i++)
    {
        snowmanList[i] = new E131Network(snowmanIP, i+100, 510);
        if (sendDMX)
        {
            networkSnowman->addNetwork(snowmanList[i]);
        }
    }

    //setup Snowmen
    this->snowmen = new Snowmen(this->skipTimeCheck, snowmanList);
    networkSnowman->setClientLock(this->snowmen->getLock());
    networkClock->setClientLock(this->getSign()->getLock());
    //


    /*
     * Setup Garage Sign
    */
    garageSign = new GarageSign(p10,mqtt);
    networkP10->setClientLock(this->garageSign->getLock());

}

GregMQTT* DisplayModel::getMqtt()
{
    return this->mqtt;
}

void DisplayModel::shutdown()
{
    printf("Shutting Down model\n");
    networks->doShutdown();
    networkClock->doShutdown();
    networkSnowman->doShutdown();
    sleep(1); // Give system time to clean up
}

bool DisplayModel::isSkipTimeCheck()
{
    return this->skipTimeCheck;
}

bool DisplayModel::isNewYears()
{
    return this->newYears;
}


CountdownClock* DisplayModel::getClock()
{
    return clock;
}

void DisplayModel::setMessage(int i, char * msg)
{
    if (i < 0 ||  i >= NUM_MESSAGE_BUFFERS)
    {
        throw "Invalid Message ID";
    }

    strncpy(messages[i], msg, MAX_MESSAGE_LENGTH);
}

char *DisplayModel::getMessage(int i)
{
    if (i <0 || i >= NUM_MESSAGE_BUFFERS)
    {
        throw "Invalid Message ID";
    }
    return messages[i];
}

GarageSign *DisplayModel::getGarageSign() {
    return garageSign;
}


Snowmen * DisplayModel::getSnowmen()
{
    return snowmen;
}

Sign * DisplayModel::getSign()
{
    return sign;
}

DisplayModel::~DisplayModel()
{
    //dtor
    // Really should remove messages.....
}
