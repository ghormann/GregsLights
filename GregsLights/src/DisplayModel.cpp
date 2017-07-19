#include "../include/DisplayModel.h"
#include "../include/controller/RGBLight.h"
#include "../include/controller/OpenDMXNetwork.h"
#include "../include/controller/LORNetwork.h"
#include "../include/controller/E131Network.h"
#include "../include/controller/NetworkCollection.h"
#include "../include/controller/Bush.h"
#include "../include/Sign.h"
#include <string.h>



DisplayModel::DisplayModel(bool sendDMX, int skip_time_check, int show_new_year)
{
    int signId = 0;
    this->skipTimeCheck = (skip_time_check == TRUE? true : false);
    this->newYears = (show_new_year == TRUE ? true: false);

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


    //LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB0", sendDMX);
    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyS0", sendDMX);


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

    // Old DMX port
    //E131Network *clockE131 = new E131Network(signIP, 100, 512); // Universe 100 of Sign Controller

    if (sendDMX)
    {
        //networks->addNetwork(clockE131);
        networks->addNetwork(lor);
        networks->addNetwork(clockNetwork[0]);
        networks->addNetwork(clockNetwork[1]);
        for (int j = 0; j< SIGN_E11_COUNT; j++)
        {
            networkClock->addNetwork(sign[j]);
        }

    }

    this->sign = new Sign(skipTimeCheck, newYears, sign);


    //setup Snowmen
    this->snowmen = new Snowmen(this->skipTimeCheck);
    for (int i =0; i < 8; i++)
    {
        snowmen->setBulb(i, lor->getBulb(5,i+1));
        snowmen->setBulb(i+8, lor->getBulb(6,i+1));
    }


    //setup Clock
    this->clock = new CountdownClock(this->skipTimeCheck, this->newYears, clockNetwork);

}

void DisplayModel::shutdown()
{
    printf("Shutting Down model\n");
    networks->doShutdown();
    networkClock->doShutdown();
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
