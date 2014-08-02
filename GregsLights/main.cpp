/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "include/RGBLight.h"
#include "include/OpenDMXNetwork.h"
#include "include/LORNetwork.h"
#include "include/NetworkCollection.h"

using namespace std;

NetworkCollection *networks;

void testDMX(OpenDMXNetwork *dmx)
{
    int i,j,k;
    int step = 25;
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
}

void testLOR(LORNetwork *lor)
{
    int i,j;
    int step = 1;
    int box = 1;
    int maxbulbs = 8*5;
    Bulb *bulbs[maxbulbs+1];

    printf("Waiting or LOR to stabalize\n");
    sleep(1);
    for (j = 1; j <=maxbulbs; j++)
    {
        if (j%8 == 0) {
            ++box;
            printf("Working on box %d\n", box);
        }
        bulbs[j] = lor->getBulb(box,j%8+1);

    }

    while(1)
    {
        for (i = 0; i <= 100; i+= step)
        {
            for (j = 1; j<=maxbulbs; j++)
            {
                bulbs[j]->setIntensity(rand() %101);
                //printf("Light %d: %d\n", j, i);
            }
            sleep(1);

        }
    }

}

int main()
{
    networks = new NetworkCollection();
    OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0");
    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB1");
    networks->addNetwork(dmx);
    networks->addNetwork(lor);
    printf("Network Established\n");
    //testDMX(dmx);
    testLOR(lor);

    return 0;
}
