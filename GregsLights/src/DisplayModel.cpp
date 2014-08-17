#include "../include/DisplayModel.h"
#include "../include/controller/RGBLight.h"
#include "../include/controller/OpenDMXNetwork.h"
#include "../include/controller/LORNetwork.h"
#include "../include/controller/NetworkCollection.h"

DisplayModel::DisplayModel(bool sendDMX)
{
    networks = new NetworkCollection();
    OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", ACTIDONGLE, sendDMX);
    //OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", OPENDMX);

    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB1", sendDMX);
    networks->addNetwork(dmx);
    networks->addNetwork(lor);

    //set up houses
    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        int j = i*3+1;
        house[i] = dmx->getRGB(j);
    }

    //setup Clock
    this->clock = new CountdownClock();
    for (int i = 0; i < CLOCK_DIGITS; i++) {
        for (int j = 0; j <7; j++) {
            this->clock->setBulb(i,j, dmx->getBulb(50+(i*7)+j));
        }
    }
    this->clock->setActive(true);
}


RGBLight* DisplayModel::getHouse(int id) {
    if (id < HOUSE_LIGHT_START) {
        throw "House ID too Low\n";
    }

    if (id > HOUSE_LIGHT_END) {
        throw "House ID too High\n";
    }

    return house[id];
}

DisplayModel::~DisplayModel()
{
    //dtor
}
