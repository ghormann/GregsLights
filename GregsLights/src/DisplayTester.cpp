#include "../include/DisplayTester.h"
#include <stdio.h>
#include <stdlib.h>



DisplayTester::DisplayTester(DisplayModel *m)
{
    this->model = m;
    //ctor
}

void DisplayTester::testDMX()
{
    int i,j;
    int maxHouse = 4;
    RGBLight *house[maxHouse + 1];
    for (i = 1; i <=4; i++)
    {
        house[i] = model->getHouse(i);
    }
    sleep(1);

    //printf("Fade up\n");
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


    //printf("Purple\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(PURPLE);
    }

    sleep(2);
    //printf("Red\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(RED);
    }
    sleep(2);
    //printf("Blue\n");
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
        //printf("Random Loop: %d  \n", i);
        house[(i%4)+1]->set(rand()%101,rand()%101,rand()%101);
        usleep(50*1000);
    }


    for (i = 0; i < 100; i++)
    {
        //printf("Loop: %d  \n", i);
        house[(i%4)+1]->set(i,0,i);
        usleep(150*1000);
    }

    sleep(5);

    for (i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++) {
        house[i]->turnOff();
    }

    sleep(1);
}


DisplayTester::~DisplayTester()
{
    //dtor
}
