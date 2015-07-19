#include "../include/LargeGrid.h"

LargeGrid::LargeGrid(E131Network *net[])
{
    //ctor
    int cnt = 0;
    for (int i = 0; i < TOTAL_GRID_PIXALS; i++)
    {
        this->pixals[cnt++] = net[0]->getRGB(i*3); // port 1
    }
}

void LargeGrid::test()
{
    int i;
    while(1)
    {
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->set(RGBColor::BLACK);
        }
        sleep(1);
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(100,0,0,20);
            //this->pixals[i]->set(RGBColor::RED);
        }
        sleep(21);
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,0.25);
            //this->pixals[i]->set(RGBColor::GREEN);
        }
        sleep(1);
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,100,0,0.25);
            //this->pixals[i]->set(RGBColor::GREEN);
        }
        sleep(1);
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,10);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(11);
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,100,1.4);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(2);
        for (i = 0; i < TOTAL_GRID_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,20);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(22);
    }
}

void LargeGrid::run() {
    test();
}

LargeGrid::~LargeGrid()
{
    //dtor
}
