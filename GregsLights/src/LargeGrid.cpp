#include "../include/LargeGrid.h"
#include <stdio.h>
#include <string.h>
#include "Generator.h"
#include "../include/controller/DummyBulb.h"
#include "RGBPicture.h"
#include <vector>
#include <iostream>

using namespace std;


LargeGrid::LargeGrid(DDPOutput *net) : GenericGrid(LGRID_PIXAL_WIDTH,LGRID_PIXAL_HEIGHT,LGRID_DUMMY_WIDTH,LGRID_DUMMY_HEIGHT)
{
    //ctor
    sprintf(message, "Booting up: Grid");

    int x = 0;
    int y = LGRID_PIXAL_HEIGHT -1;
    int networkPixel = 0;
    int dir = -1;  /* 1 = Down, -1 = up */

    /*
     * Remove this when done
     */
    for (int i = 0; i < LGRID_TOTAL_PIXALS; i++)
    {
        this->pixals[i] = NULL;
    }

    /*
     * Do the first Controller forward
     */
    for (int i = 0; i < (LGRID_TOTAL_PIXALS - (650*4)); i++)
    {
        if (y == LGRID_PIXAL_HEIGHT)
        {
            x++;
            dir = -1;
            --y;
        }
        else if (y == -1)
        {
            x++;
            dir = 1;
            y = 0;
        }

        //printf("x: %d, y: %d, networkPixal: %d, i: %d\n", x,y,networkPixel, i);
        this->pixals[x * LGRID_PIXAL_HEIGHT + y] = net->getRGB(networkPixel);
        y += dir;
        ++networkPixel;
    }

    /*
     * Do the last Controller backwards
     */
    x= LGRID_PIXAL_WIDTH -1;
    y= LGRID_PIXAL_HEIGHT-1;
    dir = -1;
    int counter = 0;
    for (int i = LGRID_TOTAL_PIXALS-1; i >= (LGRID_TOTAL_PIXALS - (650*4)); i--)
    {
        if (y == LGRID_PIXAL_HEIGHT)
        {
            x--;
            dir = -1;
            --y;
        }
        else if (y == -1)
        {
            x--;
            dir = 1;
            y = 0;
        }
        ++counter;

        //printf("x: %d, y: %d, networkPixal: %d - BACKWARDS\n", x,y,networkPixel);
        this->pixals[x * LGRID_PIXAL_HEIGHT + y] = net->getRGB(networkPixel);
        y += dir;
        ++networkPixel;

    }


    // Check and make sure everything is set
    for (int i = 0; i < LGRID_TOTAL_PIXALS; i++)
    {
        if (pixals[i] == NULL)
        {
            printf("WARNING: %d is NULL\n", i);
        }
    }
    //printf("All Good\n");

    // Check it
    for (int i =0; i < LGRID_TOTAL_PIXALS; i++)
    {
        //printf("Testing: %d\n", i);
        this->pixals[i]->set(0,0,0);
    }

    // Setup Dummy Pials
    for (int i = 0 ; i < (LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
    }

}


void LargeGrid::test()
{
    while(0)
    {
        setDummyBackground(RGBColor::BLACK,0,0,gridWidth,gridHeight+3);
        RGBColor *color = RGBColor::getRandom();
        writeText(color,13,0,"READY");
        writeText(color,gridWidth/2-14,15,"TO");
        writeText(color,10,30,"COUNT?");
        setDisplayPosition(0,3);
        gridSleep(0.25);

    }

    //Roate Colors
    while(0)
    {
        for (int x = 0; x < LGRID_PIXAL_WIDTH; x++)
        {
            for (int y = 0; y < LGRID_PIXAL_HEIGHT; y++)
            {
                this->getPixal(x,y)->set(100,0,0);
            }
        }

        gjhSleep(5);

        for (int x = 0; x < LGRID_PIXAL_WIDTH; x++)
        {
            for (int y = 0; y < LGRID_PIXAL_HEIGHT; y++)
            {
                this->getPixal(x,y)->set(0,100,0);
            }
        }

        gjhSleep(1);
    }

    // show lines
    while(1)
    {
        for (int i = 0; i < LGRID_PIXAL_WIDTH; i++)
        {
            int row = i%LGRID_PIXAL_HEIGHT;
            for (int j =0; j < LGRID_PIXAL_WIDTH; j++)
            {
                getPixal(j,row)->set(RGBColor::RED);
                if (row > 1)
                    getPixal(j,row-1)->set(RGBColor::GREEN);
                if (row > 2)
                    getPixal(j,row-2)->set(RGBColor::BLUE);
            }

            for (int j=0; j< LGRID_PIXAL_HEIGHT; j++)
            {
                getPixal(i,j)->set(RGBColor::GREEN);
            }

            gjhSleep(0.5);
            for (int j =0; j < LGRID_PIXAL_WIDTH; j++)
            {
                getPixal(j,row)->set(15,15,15);
                if (row > 1)
                    getPixal(j,row-1)->set(15,15,15);
                if (row > 2)
                    getPixal(j,row-2)->set(15,15,15);
            }
            for (int j=0; j< LGRID_PIXAL_HEIGHT; j++)
            {
                getPixal(i,j)->set(15,15,15);
            }
        }
    }

}

char *LargeGrid::getMessage()
{
    return message;
}

RGBLight *LargeGrid::getPixal(int x, int y)
{
    int pos = 0;

    if (x < 0 || x >= LGRID_PIXAL_WIDTH)
    {
        cout << "Illegal value of X in Large Grid: " << x << endl;
        throw "Illegal Value of X in getPixal";
    }

    if (y < 0 || y >= LGRID_PIXAL_HEIGHT)
    {
        cout << "Illegal value of Y in Large Grid: " << y << endl;
        throw "Illegal Value of Y in getPixal";
    }

    pos = x * LGRID_PIXAL_HEIGHT + y;

    return this->pixals[pos];
}



RGBLight *LargeGrid::getBoard(int x, int y)
{
    int pos = y*LGRID_DUMMY_WIDTH + x;

    if (x <0 || x >= LGRID_DUMMY_WIDTH)
    {
        cout << "Illegal Value for X in getBoard (Large Grid)" << endl;
        throw "Illegal Value for X in getBoard";
    }

    if (y <0 || y >= LGRID_DUMMY_HEIGHT)
    {
        cout << "Illegal Value for Y in getBoard (Large Grid)" << endl;
        throw "Illegal Value for Y in getBoard";
    }

// TODO (ghormann#1#): THis should be removed for production.  It isn't necessary.

    if (pos > (LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH))
    {
        cout << "Sign:getBoard: Invalid x and y positions (Large Grid)" << endl;
        throw "Sign:getBoard: Invalid x and y positions";
    }

    return board[pos];
}

void LargeGrid::run()
{
    while (! TimeInfo::getInstance()->isDisplayHours())
    {
        sprintf(message, "Sleeping during day (%02d)",
                TimeInfo::getInstance()->getHourOfDay());
        setBackground(RGBColor::BLACK);
        gridSleep(5);;
    }

    int numSeconds = timeInfo->getSecondsUntil();
    if (numSeconds < 33 && numSeconds > 0)
    {
        sprintf(message, "Booting up: Grid");
//        interruptAble = false;
//        countdown();
//       interruptAble = true;
    }
    else
    {
        sprintf(message, "Booting up: Grid");
    }
}

LargeGrid::~LargeGrid()
{
    //dtor
}

