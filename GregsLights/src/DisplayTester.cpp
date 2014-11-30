#include "../include/DisplayTester.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


DisplayTester::DisplayTester(DisplayModel *m)
{
    this->model = m;
    //ctor
}

void DisplayTester::testAll()
{
    pthread_t clock_t;
    pthread_t dmx_t;
    pthread_t sign_t;
    pthread_t bush_t;
    pthread_create(&(clock_t), NULL, DisplayTester::clockThread, (void*) this);
    pthread_create(&(dmx_t), NULL, DisplayTester::dmxThread, (void*) this);
    pthread_create(&(sign_t), NULL, DisplayTester::signThread, (void*) this);
    pthread_create(&(bush_t), NULL, DisplayTester::bushThread, (void*) this);

    // Join disabled becaue the graphics byild needs testAll to return....
    //pthread_join(clock_t, NULL);
    //pthread_join(dmx_t, NULL);
    //pthread_join(sign_t, NULL);
}

void DisplayTester::testSign()
{
    this->model->getSign()->test();
}

void * DisplayTester::clockThread(void *args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    ptr->testClock();
    return NULL;
}

void * DisplayTester::signThread(void *args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    ptr->testSign();
    return NULL;
}

void * DisplayTester::dmxThread(void *args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    while(1)
    {
        ptr->testDMX();
    }

    return NULL;
}

void *DisplayTester::bushThread(void *args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    while(1) {
        ptr->testBushes();
    }
    return NULL;
}


void DisplayTester::testClock()
{
    //this->model->getClock()->testALlOn();
    this->model->getClock()->test();
    //this->model->getClock()->setActive(true);
    while(1)
    {
        sleep(100);  // Needed because setActive starts its own thread.
    }
}

void DisplayTester::testBushes()
{
    while(1)
    {
        for (int i=BUSH_LIGHT_START; i<= BUSH_LIGHT_END; i++)
        {
            model->getBush(i)->getRed()->setIntensity(100);
            model->getBush(i)->getWhite()->setIntensity(0);
        }
        sleep(1);
        for (int i=BUSH_LIGHT_START; i<=BUSH_LIGHT_END; i++)
        {
            model->getBush(i)->getRed()->setIntensity(0);
            model->getBush(i)->getGreen()->setIntensity(100);
        }
        sleep(1);
        for (int i=BUSH_LIGHT_START; i<=BUSH_LIGHT_END; i++)
        {
            model->getBush(i)->getGreen()->setIntensity(0);
            model->getBush(i)->getBlue()->setIntensity(100);
        }
        sleep(1);
        for (int i=BUSH_LIGHT_START; i<=BUSH_LIGHT_END; i++)
        {
            model->getBush(i)->getBlue()->setIntensity(0);
            model->getBush(i)->getWhite()->setIntensity(100);
        }
        sleep(1);
    }
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

    model->setMessage(1, "Fade Up");
    for (j=1; j<=4; j++)
    {
        for (i=0; i<=100; i++)
        {
            house[j]->set(i,0,0);
            usleep(30000);
        }
        for (i=0; i<=100; i++)
        {
            house[j]->set(0,i,0);
            usleep(30000);
        }
        for (i=0; i<=100; i++)
        {
            house[j]->set(0,0,100-i);
            usleep(30000);
        }
    }

    model->setMessage(1, "Light 1: 10,10,10");
    house[1]->set(10,10,10);
    sleep(3);


    model->setMessage(1, "Purple\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(PURPLE);
    }

    sleep(2);
    model->setMessage(1, "Red\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(RED);
    }
    sleep(2);
    model->setMessage(1, "Blue\n");
    for(i = 1; i<=4; i++)
    {
        house[i]->setStdColor(BLUE);
    }


    sleep(2);
    model->setMessage(1, "Multi Color");
    house[1]->set(50,0,50);
    house[2]->set(25,50,75);
    house[3]->set(100,100,100);
    house[4]->set(100,100,100);

    sleep(5);

    for (i = 0; i < 1000; i++)
    {
        sprintf(model->getMessage(1), "Random Loop: %d  \n", i);
        house[(i%4)+1]->set(rand()%101,rand()%101,rand()%101);
        usleep(50*1000);
    }


    for (i = 0; i < 100; i++)
    {
        sprintf(model->getMessage(1),  "Loop: %d  \n", i);
        house[(i%4)+1]->set(i,0,i);
        usleep(150*1000);
    }

    sleep(5);

    model->setMessage(1, "House off\n");
    for (i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        house[i]->turnOff();
    }

    sleep(1);
}


DisplayTester::~DisplayTester()
{
    //dtor
}
