#include "../include/LargeGrid.h"
#include <stdio.h>
#include <string.h>


LargeGrid::LargeGrid(E131Network *net[])
{
    //ctor
    sprintf(message, "Booting up: Grid");

    int network = 0;
    int cnt=0;
    // TODO: his really needs changed, but for testing it is good enough
    for (int i = 0; i < TOTAL_GRID_PIXALS; i++)
    {
        if (cnt==170)
        {
            cnt = 0;
            ++network;
            //printf("i: %d, CNT: %d, network: %d\n", i,cnt,network);
        }
        if (i == 2700) { // Force network swtich
            cnt = 0;
            ++network;
            //printf("i: %d, CNT: %d, network: %d\n", i,cnt,network);
        }
        this->pixals[i] = net[network]->getRGB(cnt*3);
        ++cnt;
    }
}

void LargeGrid::test()
{
    int i;
    while(1)
    {
        sprintf(message, "Set Black");
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->set(RGBColor::BLACK);
        }
        sleep(1);
        //sprintf(message, "Set fade Red 20, %d, %d", sizeof(Bulb), sizeof(FadeableBulb));
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(100,0,0,20);
            //this->pixals[i]->set(RGBColor::RED);
        }
        sleep(21);
        sprintf(message, "Set fade Black 0.25");
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,0.25);
            //this->pixals[i]->set(RGBColor::GREEN);
        }
        sleep(1);
        sprintf(message, "Set fade Green 1.25");
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,100,0,1.25);
            //this->pixals[i]->set(RGBColor::GREEN);
        }
        sleep(2);
        sprintf(message, "Set fade Black 10");
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,10);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(11);
        sprintf(message, "Set fade BLUE 2.0");
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,100,2.0);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(2);
        sprintf(message, "Set fade BLACK 4");
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,4);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(4);
    }
}

char *LargeGrid::getMessage()
{
    return message;
}


void LargeGrid::run()
{
    test();
}

LargeGrid::~LargeGrid()
{
    //dtor
}
