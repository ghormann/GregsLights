#include "../include/GregsDisplay.h"
#include "../include/controller/Bush.h"
#include <string.h>
#include <iostream>

#define RED 0
#define GREEN 1
#define BLUE 2
#define WHITE 3

using namespace std;

GregsDisplay::GregsDisplay(DisplayModel *m)
{
    this->model = m;
    timeinfo = TimeInfo::getInstance();
    timeinfo->setSkipTimeCheck(m->isSkipTimeCheck());
    timeinfo->setNewYear(m->isNewYears());
}

GregsDisplay::~GregsDisplay()
{
    //dtor
}


void GregsDisplay::run()
{
    pthread_t sign_t;
    pthread_t snowmen_t;
    pthread_create(&(sign_t), NULL, GregsDisplay::signThread, (void*) this);
    pthread_create(&(snowmen_t), NULL, GregsDisplay::snowmenThread, (void*) this);

    getModel()->getClock()->setActive(true);
}

DisplayModel *GregsDisplay::getModel()
{
    return model;
}


void * GregsDisplay::snowmenThread(void * args)
{
    GregsDisplay *ptr = (GregsDisplay *) args;
    while(1)
    {
        ptr->getModel()->getSnowmen()->run();
    }
    return NULL;
}


void * GregsDisplay::signThread(void *args)
{
    GregsDisplay *ptr = (GregsDisplay *) args;
    while (1)
    {
        ptr->getModel()->getSign()->run();
    }
    return NULL;
}

