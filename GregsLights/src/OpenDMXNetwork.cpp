#include "../include/OpenDMXNetwork.h"
#include <stdio.h>

OpenDMXNetwork::OpenDMXNetwork(char * deviceName)
{
    for (int i =0; i <513; i++)
    {
        this->data[i] = 0;
    }
}

IPixal* OpenDMXNetwork::getPixal(int channel)
{
    if (channel < 0 || channel > 512)
        throw "Invalid Channel";
    DMXPixal *rc = new DMXPixal(data+channel);
    return  rc;
}

void OpenDMXNetwork::doUpdate() {
    char greg[600];
    for (int j = 0; j < 10; j++)
    {
        sprintf(greg+2*j, "%02X", data[j]);
        greg[20] = '\0';
    }
    printf("Data: %s\n", greg);
}

RGBLight* OpenDMXNetwork::getRGB(int start) {
    IPixal *r = this->getPixal(start);
    IPixal *g = this->getPixal(start+1);
    IPixal *b = this->getPixal(start+2);
    return new RGBLight(r,g,b);
}

OpenDMXNetwork::~OpenDMXNetwork()
{
    //dtor
}

void DMXPixal::setIntensity_ipml(int val)
{
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    pos[0] = (char) val;
}
