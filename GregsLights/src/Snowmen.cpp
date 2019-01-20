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

void Snowmen::drawSnowmen(int pos)
{
    int x,y;
    GenericGrid * who = getSnowmen(pos);
    //TOP
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-65,10,RGBColor::WHITE);
    //Mid
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-45,14,RGBColor::WHITE);
    //Bottom
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-20,20,RGBColor::WHITE);

    //Nose
    int startX = SNOWMEN_WIDTH/2+8;
    int endX = SNOWMEN_WIDTH/2+16;
    y = SNOWMEN_HEIGHT-65;
    if (pos == SNOWMAN_RIGHT)
    {
        startX -=23;
        endX -=23;
    }

    for (x = startX; x < endX; x++)
    {
        who->getPixal(x,y)->set(RGBColor::ORANGE);
        who->getPixal(x,y-1)->set(RGBColor::ORANGE);
    }
    // Eye
    x = (pos==SNOWMAN_LEFT ? SNOWMEN_WIDTH/2+5 : SNOWMEN_WIDTH/2-4);
    who->drawCircle(x,SNOWMEN_HEIGHT-70,1,RGBColor::BLACK);
    //Buttons
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-26,1,RGBColor::BLACK);
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-38,1,RGBColor::BLACK);
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-50,1,RGBColor::BLACK);

    // Mouth
    if (pos == SNOWMAN_LEFT)
    {
        x=SNOWMEN_WIDTH/2+8;
        y=SNOWMEN_HEIGHT-60;
        who->getPixal(x,y)->set(RGBColor::BLACK);
        who->getPixal(x-1,y)->set(RGBColor::BLACK);
        who->getPixal(x-2,y-1)->set(RGBColor::BLACK);
        who->getPixal(x-3,y-2)->set(RGBColor::BLACK);
    }
    else
    {
        x=SNOWMEN_WIDTH/2-8;
        y=SNOWMEN_HEIGHT-60;
        who->getPixal(x,y)->set(RGBColor::BLACK);
        who->getPixal(x+1,y)->set(RGBColor::BLACK);
        who->getPixal(x+2,y-1)->set(RGBColor::BLACK);
        who->getPixal(x+3,y-2)->set(RGBColor::BLACK);
    }
}

void DrawLine(GenericGrid *grid, int startX, int endX, double mx, int b, RGBColor *color)
{
    int x,y;
    for (x = startX; x<=endX; x++)
    {
        y = mx*x + b;
        grid->getPixal(x,y)->set(color);
    }
}

void Snowmen::throwLeft()
{
    const int sholder_x = 16;
    const int sholder_y = 42;
    const int ball_x[] = {12,12,11, 10, 9, 8, 8, 7, 7, 6, 8, 10, 12, 14, 16, 19, 22, 25, 28, 32, 36, 40};
    const int ball_y[] = {16,16,16,16, 15, 15, 15, 14, 14, 13, 13, 12, 12, 11, 12, 12, 12, 12, 13, 14, 15, 17};
    const int elbow_x[] = {1,2,3, 4, 5, 5, 6, 7, 8, 9};
    const int elbow_y[] = {30,30,30, 30, 30, 30, 30, 30, 30, 30};

    // For Arm away
    const int hand_x[] = {41, 42, 43, 44, 43, 42};
    const int hand_y[] = {20, 26, 32, 38, 44, 50};
    const int ball2_x[] = {44,48, 0, 3, 6, 9, 12, 15, 18, 21, 23};
    const int ball2_y[] = {17,17, 8, 8, 8, 8,  8,  8,  8, 8, 8};
    const double duration = 0.02;

    GenericGrid *left = getSnowmen(SNOWMAN_LEFT);
    GenericGrid *splash = getSplashGrid(SNOWMAN_LEFT);

    // Pull back the Arm
    for (int i = 0; i< 10; i++)
    {
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT);

        left->plotLineWidth(elbow_x[i],elbow_y[i],sholder_x,sholder_y,2.0,RGBColor::WHITE);
        left->plotLineWidth(elbow_x[i],elbow_y[i],ball_x[i],ball_y[i],2.0,RGBColor::WHITE);
        left->drawCircle(ball_x[i],ball_y[i],3, RGBColor::WHITE);
        write_data(duration);
    }

    // Move the Arm foward as a Straight line
    for (int i = 10; i< 22; i++)
    {
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT);

        left->plotLineWidth(sholder_x,sholder_y,ball_x[i],ball_y[i],2.0,RGBColor::WHITE);
        left->drawCircle(ball_x[i],ball_y[i],3, RGBColor::WHITE);
        write_data(duration);
    }

    // Move Arm Away
    for (int i = 0; i< 6; i++)
    {
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT);

        left->plotLineWidth(sholder_x,sholder_y,hand_x[i],hand_y[i],2.0,RGBColor::WHITE);
        if (i < 2)
        {
            left->drawCircle(ball2_x[i],ball2_y[i],3, RGBColor::WHITE);
        }
        else
        {
            splash->drawCircle(ball2_x[i],ball2_y[i],1, RGBColor::WHITE);
        }
        write_data(duration);
        if (i>=2)
        {
            splash->drawCircle(ball2_x[i],ball2_y[i],1, RGBColor::BLACK);
        }
    }

    // Throw slpash
    for (int i = 6; i < 11; i++)
    {
        splash->drawCircle(ball2_x[i],ball2_y[i],1, RGBColor::WHITE);
        write_data(duration);
        splash->drawCircle(ball2_x[i],ball2_y[i],1, RGBColor::BLACK);
    }

    write_data(1.0);

}

void Snowmen::do_it_snowmen()
{
    drawSnowmen(SNOWMAN_LEFT);
    drawSnowmen(SNOWMAN_RIGHT);
    while(1)
    {
        throwLeft();
    }
    //sleep(5); // Replace me
}


char * Snowmen::getMessage()
{
    return message2;
}
