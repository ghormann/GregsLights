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
    int i,j;
    int step = 25;
    int maxHouse = 4;
    RGBLight *house[maxHouse + 1];
    for (i = 1; i <=4; i++)
    {
        int j = i*3+1;
        printf("Light %d is %d\n", i, j);
        house[i] = dmx->getRGB(j);
    }
    sleep(1);

    printf("Fade up\n");
    for (j=1; j<=4; j++)
    {
        for (i=0; i<100; i++)
        {
            house[j]->set(i,0,0);
            usleep(30000);
        }
        for (i=0; i<100; i++)
        {
            house[j]->set(0,i,0);
            usleep(30000);
        }
        for (i=0; i<100; i++)
        {
            house[j]->set(0,0,100-i);
            usleep(30000);
        }
    }

    house[1]->set(10,10,10);
    sleep(3);


    printf("Purple\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(PURPLE);
    }

    sleep(2);
    printf("Red\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(RED);
    }
    sleep(2);
    printf("Blue\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(BLUE);
    }


    sleep(2);
    house[1]->set(50,0,50);
    house[2]->set(25,50,75);
    house[3]->set(100,100,100);
    house[4]->set(100,100,100);

    sleep(5);

    for (i = 0; i < 1000; i++)
    {
        printf("Random Loop: %d  \n", i);
        house[(i%4)+1]->set(rand()%101,rand()%101,rand()%101);
        usleep(50*1000);
    }


    for (i = 0; i < 100; i++)
    {
        printf("Loop: %d  \n", i);
        house[(i%4)+1]->set(i,0,i);
        usleep(150*1000);
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
        if (j%8 == 0)
        {
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
    OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", ACTIDONGLE);
    //OpenDMXNetwork *dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0", OPENDMX);
    LORNetwork *lor = new LORNetwork((char*) "/dev/ttyUSB1");
    networks->addNetwork(dmx);
    networks->addNetwork(lor);
    printf("Network Established\n");
    testDMX(dmx);
    //testLOR(lor);

    return 0;
}
