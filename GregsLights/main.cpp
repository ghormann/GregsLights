/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include <stdio.h>
#include "include/RGBLight.h"
#include "include/OpenDMXNetwork.h"
#include "include/LORNetwork.h"
#include "include/NetworkCollection.h"

using namespace std;

NetworkCollection *networks;


int main()
{
    int i,j,k;
    int step = 25;
    networks = new NetworkCollection();
    OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0");
    LORNetwork *lor = new LORNetwork((char*) "/dev/usb005");
    networks->addNetwork(dmx);
    networks->addNetwork(lor);

    RGBLight *light1 = dmx->getRGB(7);
    RGBLight *light2 = dmx->getRGB(10);
    RGBLight *light3 = dmx->getRGB(50);
    light1->setStdColor(PURPLE);
    sleep(5);
    light2->set(25,50,75);
    light3->set(100,100,100);

    light1->set(50,0,50);
    sleep(5);

    for (i = 0; i < 100; i++)
    {
        printf("Loop: %d  \n", i);
        light1->set(i,0,i);
        usleep(150*1000);
    }

    for (i = 0; i <= 100; i+= step)
    {
        for (j=0; j <= 100; j+= step)
        {
            for (k=0; k <= 100; k += step)
            {
                printf("Loop: %d  %d  %d\n", i,j,k);
                light1->set(i,j,k);
                sleep(1);

            }
        }
    }

    sleep(5);
    return 0;
}
