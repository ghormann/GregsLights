#include "../include/Snowmen.h"
#include "../include/controller/DummyBulb.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>




Snowmen::Snowmen(bool skipTime)
{
    //ctor
    timeinfo = new TimeInfo(skipTime,false);
    snowman_t = 0;
    snowmen[0] = new SnowmenGrid(SNOWMEN_WIDTH,SNOWMEN_HEIGHT,SNOWMEN_WIDTH, SNOWMEN_HEIGHT,skipTime,false);
    snowmen[1] = new SnowmenGrid(SNOWMEN_WIDTH,SNOWMEN_HEIGHT,SNOWMEN_WIDTH, SNOWMEN_HEIGHT,skipTime,false);
    splash[0] = new SnowmenGrid(SPLASH_GRID_WIDTH,SPLASH_GRID_HEIGHT,SPLASH_GRID_WIDTH, SPLASH_GRID_HEIGHT,skipTime,false);
    splash[1] = new SnowmenGrid(SPLASH_GRID_WIDTH,SPLASH_GRID_HEIGHT,SPLASH_GRID_WIDTH, SPLASH_GRID_HEIGHT,skipTime,false);
    skyGrid = new SnowmenGrid(SKY_GRID_WIDTH,SKY_GRID_HEIGHT,SKY_GRID_WIDTH, SKY_GRID_HEIGHT,skipTime,false);
    strcpy(message2, "Starting up");
}

Snowmen::~Snowmen()
{
    //dtor
}

int SnowmenGrid::getPos(int x, int y)
{
    int xmod = x%2;
    int pos = 0;

    if (xmod == 0)
    {
        pos = x * gridHeight + y;
    }
    else
    {
        pos = x*gridHeight + (gridHeight-1) - y;
    }
    return pos;
}

void SnowmenGrid::setPixal(int x, int y, RGBLight * p)
{
    pixals[getPos(x,y)] = p;
}

RGBLight *SnowmenGrid::getPixal(int x, int y)
{
    return pixals[getPos(x,y)];
}

RGBLight *SnowmenGrid::getBoard(int x, int y)
{
    int pos = y*gridWidth + x;
    return board[pos];
}


SnowmenGrid::SnowmenGrid(int width_, int height_, int dummy_with_, int dummy_height, bool skipTime, bool newYears) : GenericGrid(width_,height_,dummy_with_,dummy_height,skipTime,newYears)
{
    // Setup Dummy Pials
    for (int i = 0 ; i < (height_ * width_); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());

// TODO (ghormann#1#): Fix Mapping of Pixals
        this->pixals[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
        this->pixals[i]->set(25,25,25);
    }

}

SnowmenGrid *  Snowmen::getSnowmen(int pos)
{
    if (pos != SNOWMAN_RIGHT && pos != SNOWMAN_LEFT)
    {
        throw "Invalid position";
    }
    return snowmen[pos];
}

SnowmenGrid *  Snowmen::getSkyGrid()
{
    return skyGrid;
}

SnowmenGrid *  Snowmen::getSplashGrid(int pos)
{
    if (pos != SNOWMAN_RIGHT && pos != SNOWMAN_LEFT)
    {
        throw "Invalid position";
    }
    return splash[pos];
}


void Snowmen::run()
{
    while(1)
    {
        if (timeinfo->isDisplayHours())
        {
            do_it_snowmen();
        }
        else
        {
            sprintf(message2, "Sleeping during day as hour is %d", timeinfo->getHourOfDay());
            //setBodies(false);
            sleep(60);
        }
    }
}

void Snowmen::do_it_snowmen()
{
    int x,y;
    GenericGrid * left = getSnowmen(SNOWMAN_LEFT);
    //TOP
    left->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-65,10,RGBColor::WHITE);
    //Mid
    left->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-45,14,RGBColor::WHITE);
    //Bottom
    left->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-20,20,RGBColor::WHITE);

    //Nose
    y = SNOWMEN_HEIGHT-65;
    for (x = +SNOWMEN_WIDTH/2+8; x < +SNOWMEN_WIDTH/2+16; x++)
    {
        left->getPixal(x,y)->set(RGBColor::ORANGE);
        left->getPixal(x,y-1)->set(RGBColor::ORANGE);
    }
    // Eye
    left->drawCircle(SNOWMEN_WIDTH/2+5,SNOWMEN_HEIGHT-70,1,RGBColor::BLACK);
    //Buttons
    left->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-26,1,RGBColor::BLACK);
    left->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-38,1,RGBColor::BLACK);
    left->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-50,1,RGBColor::BLACK);

    // Mouth

    x=SNOWMEN_WIDTH/2+8;
    y=SNOWMEN_HEIGHT-60;
    left->getPixal(x,y)->set(RGBColor::BLACK);
    left->getPixal(x-1,y)->set(RGBColor::BLACK);
    left->getPixal(x-2,y-1)->set(RGBColor::BLACK);
    left->getPixal(x-3,y-2)->set(RGBColor::BLACK);


    sleep(5); // Replace me
}


char * Snowmen::getMessage()
{
    return message2;
}
