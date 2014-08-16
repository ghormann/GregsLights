#include "../include/DisplayModel.h"
#include "../include/RGBLight.h"
#include "../include/OpenDMXNetwork.h"
#include "../include/LORNetwork.h"
#include "../include/NetworkCollection.h"

DisplayModel::DisplayModel(bool sendDMX)
{
    networks = new NetworkCollection();
    OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", ACTIDONGLE);
    //OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", OPENDMX);

    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB1");
    networks->addNetwork(dmx);
    networks->addNetwork(lor);

    //set up houses
    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        int j = i*3+1;
        house[i] = dmx->getRGB(j);
    }
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
