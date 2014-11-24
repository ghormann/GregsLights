#include "../include/DisplayModel.h"
#include "../include/controller/RGBLight.h"
#include "../include/controller/OpenDMXNetwork.h"
#include "../include/controller/LORNetwork.h"
#include "../include/controller/E131Network.h"
#include "../include/controller/NetworkCollection.h"
#include "../include/controller/Bush.h"
#include <string.h>

DisplayModel::DisplayModel(bool sendDMX)
{
    //Initialize Memory Buffers
    for (int i = 0; i < NUM_MESSAGE_BUFFERS; i++)
    {
        messages[i] = new char[MAX_MESSAGE_LENGTH+1];
        messages[i][0] = 0;
    }

    //
    networks = new NetworkCollection();
    OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", ACTIDONGLE, sendDMX);
    //OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", OPENDMX);

    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB1", sendDMX);

    E131Network *sign = new E131Network("192.168.0.205", 100, 512);

    networks->addNetwork(dmx);
    networks->addNetwork(lor);
    networks->addNetwork(sign);

    //set up houses
    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        int j = i*3+1;
        house[i] = dmx->getRGB(j);
    }

    // setup Bushes;
    bushes[1] = new Bush(1,lor->getBulb(1,1), lor->getBulb(1,2), lor->getBulb(1,3), lor->getBulb(1,4));
    bushes[2] = new Bush(2,lor->getBulb(1,5), lor->getBulb(1,6), lor->getBulb(1,7), lor->getBulb(1,8));
    bushes[3] = new Bush(3,lor->getBulb(2,1), lor->getBulb(2,2), lor->getBulb(2,3), lor->getBulb(2,4));
    bushes[4] = new Bush(4,lor->getBulb(3,1), lor->getBulb(3,2), lor->getBulb(3,3), lor->getBulb(3,4));
    bushes[5] = new Bush(5,lor->getBulb(3,5), lor->getBulb(3,6), lor->getBulb(3,7), lor->getBulb(3,8));
    bushes[6] = new Bush(6,lor->getBulb(4,1), lor->getBulb(4,2), lor->getBulb(4,3), lor->getBulb(4,4));

    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++) {
        bushes[i]->setAll(0);
    }

    //setup Clock
    this->clock = new CountdownClock();
    for (int i = 0; i < CLOCK_DIGITS; i++)
    {
        for (int j = 0; j <7; j++)
        {
            this->clock->setBulb(i,j, dmx->getBulb(50+(i*7)+j));
        }
    }
    this->clock->setActive(true);
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

DisplayModel::~DisplayModel()
{
    //dtor
    // Really should remove messages.....
}
