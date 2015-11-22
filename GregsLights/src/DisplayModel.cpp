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
    char *gridIP1 = "192.168.0.230";  //AlphaPix for Grid:  13 * 170 or 2720 Pixals(2196 used)
    char *gridIP2 = "192.168.0.231";  // AlphaPix for Grid: 16 *170 or 2210 Pixals (2700 Used for Grid )


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
    networks = new NetworkCollection("Normal", 5,25,MAX_LIGHT_NETWORKS,1);
    // name, 5, 2, 6, 15 is "OK" for Alpha Pix.
    // name: 5,2,6,5 I Fairly good.
    networkAlpha1 = new NetworkCollection("Alpha1",5,2,6,1);
    networkAlpha2 = new NetworkCollection("Alpha2",5,2,6,1);
    networkAlpha1->setControllerLimits(6,1);
    networkAlpha2->setControllerLimits(6,1);
    E131Network *dmx = new E131Network("192.168.0.230", 20,512); // AlphaPix DMX
    //OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", ACTIDONGLE, sendDMX);
    //OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", OPENDMX);

    networkAlpha1->addNetwork(dmx);

    //LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB0", sendDMX);
    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB0", false);


    /* Debug ONly */
    char *signIP = "192.168.0.232";
    E131Network *sign[SIGN_E11_COUNT];
    sign[signId++] = new E131Network(signIP, 40, 512);  // port 4 170
    sign[signId++] = new E131Network(signIP, 41, 512); // port 4 170
    sign[signId++] = new E131Network(signIP, 42, 512); // port 4 170
    sign[signId++] = new E131Network(signIP, 10, 512);  // port 1 170
    sign[signId++] = new E131Network(signIP, 11, 512);  // port 1 170
    sign[signId++] = new E131Network(signIP, 12, 512);  // port 1 170
    sign[signId++] = new E131Network(signIP, 13, 512);  // port 1 140
    sign[signId++] = new E131Network(signIP, 20, 512);  // port 2 170
    sign[signId++] = new E131Network(signIP, 21, 512);  // port 2 140
    sign[signId++] = new E131Network(signIP, 30, 512);  // port 3 170
    sign[signId++] = new E131Network(signIP, 31, 512);  // port 3 170
    sign[signId++] = new E131Network(signIP, 32, 512);  // port 3 170

    E131Network *grid1[GRID_E11_COUNT];
    E131Network *grid2[GRID_E11_COUNT];
    int universe = 1;
    for (int i = 0; i < GRID_E11_COUNT; i++)
    {
        grid1[i] = new E131Network( (gridIP1 ),universe,512);
        grid2[i] = new E131Network( (gridIP2 ),universe,512);
        ++universe;
    }

    E131Network *clockE131 = new E131Network(signIP, 100, 512); // Universe 100 of Sign Controller

    if (sendDMX)
    {
        networks->addNetwork(clockE131);
        //networks->addNetwork(lor);
        for (int j = 0; j< SIGN_E11_COUNT; j++)
        {
            networks->addNetwork(sign[j]);
        }

        for (int j=0; j<GRID_E11_COUNT; j++) // GRID_E11_COUNT
        {
            networkAlpha1->addNetwork(grid1[j]);
            networkAlpha2->addNetwork(grid2[j]);
        }
        //grid1[0]->setDebug(true);
        //sign[0]->setDebug(true);
    }

    this->stars = new StarField(dmx);
    this->sign = new Sign(skipTimeCheck, newYears, sign);
    this->grid = new LargeGrid(skipTimeCheck, newYears,grid1, grid2);

    //set up houses (Must subtract one when using E131.  It is zero based)
    house[1] = dmx->getRGB(13-1);
    house[2] = dmx->getRGB(10-1);
    house[3] = dmx->getRGB(7-1);
    house[4] = dmx->getRGB(4-1);

    //setup Snowmen
    this->snowmen = new Snowmen(this->skipTimeCheck);
    for (int i =0; i < 8; i++)
    {
        snowmen->setBulb(i, lor->getBulb(5,i+1));
        snowmen->setBulb(i+8, lor->getBulb(6,i+1));
    }

    // setup Bushes;
    bushes[1] = new Bush(1,lor->getBulb(1,1), lor->getBulb(1,2), lor->getBulb(1,3), lor->getBulb(1,4));
    bushes[2] = new Bush(2,lor->getBulb(1,5), lor->getBulb(1,6), lor->getBulb(1,7), lor->getBulb(1,8));
    bushes[3] = new Bush(3,lor->getBulb(2,1), lor->getBulb(2,2), lor->getBulb(2,3), lor->getBulb(2,4));
    bushes[4] = new Bush(4,lor->getBulb(3,1), lor->getBulb(3,2), lor->getBulb(3,3), lor->getBulb(3,4));
    bushes[5] = new Bush(5,lor->getBulb(4,1), lor->getBulb(4,2), lor->getBulb(4,3), lor->getBulb(4,4));
    bushes[6] = new Bush(6,lor->getBulb(4,5), lor->getBulb(4,6), lor->getBulb(4,7), lor->getBulb(4,8));

    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++)
    {
        bushes[i]->setAll(0);
    }

    //setup Clock
    this->clock = new CountdownClock(this->skipTimeCheck, this->newYears);
    int i = 100-1;   // Starting at the 100th spot
    this->clock->setBulb(0,0,clockE131->getBulb(i + 19));
    this->clock->setBulb(0,1,clockE131->getBulb(i + 15));
    this->clock->setBulb(0,2,clockE131->getBulb(i + 14));
    this->clock->setBulb(0,3,clockE131->getBulb(i + 20));
    this->clock->setBulb(0,4,clockE131->getBulb(i + 17));
    this->clock->setBulb(0,5,clockE131->getBulb(i + 18));
    this->clock->setBulb(0,6,clockE131->getBulb(i + 16));
    // Next
    this->clock->setBulb(1,0,clockE131->getBulb(i + 12));
    this->clock->setBulb(1,1,clockE131->getBulb(i + 10));
    this->clock->setBulb(1,2,clockE131->getBulb(i + 13));
    this->clock->setBulb(1,3,clockE131->getBulb(i + 7));
    this->clock->setBulb(1,4,clockE131->getBulb(i + 9));
    this->clock->setBulb(1,5,clockE131->getBulb(i + 6));
    this->clock->setBulb(1,6,clockE131->getBulb(i + 11));
    // Next
    this->clock->setBulb(2,0,clockE131->getBulb(i + 1));
    this->clock->setBulb(2,1,clockE131->getBulb(i + 0));
    this->clock->setBulb(2,2,clockE131->getBulb(i + 3));
    this->clock->setBulb(2,3,clockE131->getBulb(i + 5));
    this->clock->setBulb(2,4,clockE131->getBulb(i + 8));
    this->clock->setBulb(2,5,clockE131->getBulb(i + 4));
    this->clock->setBulb(2,6,clockE131->getBulb(i + 2));
    // Next
    i = 130-1;   // Next controller
    this->clock->setBulb(3,0,clockE131->getBulb(i + 2));
    this->clock->setBulb(3,1,clockE131->getBulb(i + 5));
    this->clock->setBulb(3,2,clockE131->getBulb(i + 0));
    this->clock->setBulb(3,3,clockE131->getBulb(i + 6));
    this->clock->setBulb(3,4,clockE131->getBulb(i + 3));
    this->clock->setBulb(3,5,clockE131->getBulb(i + 4));
    this->clock->setBulb(3,6,clockE131->getBulb(i + 1));
    // Next
    this->clock->setBulb(4,0,clockE131->getBulb(i + 12));
    this->clock->setBulb(4,1,clockE131->getBulb(i + 8));
    this->clock->setBulb(4,2,clockE131->getBulb(i + 13));
    this->clock->setBulb(4,3,clockE131->getBulb(i + 7));
    this->clock->setBulb(4,4,clockE131->getBulb(i + 11));
    this->clock->setBulb(4,5,clockE131->getBulb(i + 10));
    this->clock->setBulb(4,6,clockE131->getBulb(i + 9));
    // Next
    this->clock->setBulb(5,0,clockE131->getBulb(i + 14));
    this->clock->setBulb(5,1,clockE131->getBulb(i + 20));
    this->clock->setBulb(5,2,clockE131->getBulb(i + 19));
    this->clock->setBulb(5,3,clockE131->getBulb(i + 17));
    this->clock->setBulb(5,4,clockE131->getBulb(i + 15));
    this->clock->setBulb(5,5,clockE131->getBulb(i + 16));
    this->clock->setBulb(5,6,clockE131->getBulb(i + 18));
    // Specials
    this->clock->setSpecial(0, clockE131->getBulb(i+21));
    this->clock->setSpecial(1, clockE131->getBulb(i+22));
    this->clock->setSpecial(2, clockE131->getBulb(i+23));
    this->clock->setSpecial(3, clockE131->getBulb(i+24));
    this->clock->setSpecial(4, clockE131->getBulb(i+25));
    this->clock->setSpecial(5, clockE131->getBulb(i+26));
    this->clock->setSpecial(6, clockE131->getBulb(i+27));

}

void DisplayModel::shutdown()
{
    printf("Shutting Down model\n");
    networks->doShutdown();
    networkAlpha1->doShutdown();
    networkAlpha2->doShutdown();
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

Bush* DisplayModel::getBush(int i)
{
    if (i < BUSH_LIGHT_START || i > BUSH_LIGHT_END )
    {
        throw "Invalid Bush id";
    }
    return bushes[i];
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

StarField* DisplayModel::getStars()
{
    return stars;
}

RGBLight* DisplayModel::getHouse(int id)
{
    if (id < HOUSE_LIGHT_START)
    {
        throw "House ID too Low\n";
    }

    if (id > HOUSE_LIGHT_END)
    {
        throw "House ID too High\n";
    }

    return house[id];
}

Snowmen * DisplayModel::getSnowmen()
{
    return snowmen;
}

Sign * DisplayModel::getSign()
{
    return sign;
}

LargeGrid *DisplayModel::getGrid()
{
    return grid;
}

DisplayModel::~DisplayModel()
{
    //dtor
    // Really should remove messages.....
}
