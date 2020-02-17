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
    pthread_t sign_t;
    pthread_t snowmen_t;
    pthread_t garage_t;
    pthread_create(&(clock_t), NULL, DisplayTester::clockThread, (void*) this);
    pthread_create(&(sign_t), NULL, DisplayTester::signThread, (void*) this);
    pthread_create(&(snowmen_t), NULL, DisplayTester::snowmenThread, (void*) this);
    pthread_create(&(garage_t), NULL, DisplayTester::garageThread, (void*) this);

}


void * DisplayTester::snowmenThread(void * args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    while(1)
    {
        //ptr->model->getSnowmen()->run();
        ptr->model->getSnowmen()->test_snowmen();
    }
    return NULL;
}



void DisplayTester::testSign()
{
    this->model->getSign()->test();
}

void * DisplayTester::clockThread(void *args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    while (1)
    {
        ptr->testClock();
    }
    return NULL;
}

void * DisplayTester::signThread(void *args)
{
    DisplayTester *ptr = (DisplayTester *) args;
    while (1)
    {
        ptr->testSign();
    }
    return NULL;
}

void *DisplayTester::garageThread(void *args) {
    DisplayTester *ptr = (DisplayTester *) args;
    while(1) {
        ptr->testGarage();
    }

    return NULL;
}

void DisplayTester::testGarage() {
    while(1) {
        this->model->getGarageSign()->test();
    }
}

void DisplayTester::testClock()
{
    //this->model->getClock()->testALlOn();
    //this->model->getClock()->setActive(true);
    while(1)
    {
        this->model->getClock()->test();
        //sleep(100);  // Needed because setActive starts its own thread.
    }
}


DisplayTester::~DisplayTester()
{
    //dtor
}
