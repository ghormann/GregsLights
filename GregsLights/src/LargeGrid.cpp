#include "../include/LargeGrid.h"
#include <stdio.h>
#include <string.h>
#include "../include/controller/DummyBulb.h"
#include "RGBPicture.h"


LargeGrid::LargeGrid(bool skipTime, bool newYears,E131Network *net[]) : GenericGrid(LGRID_PIXAL_WIDTH,LGRID_PIXAL_HEIGHT,LGRID_DUMMY_WIDTH,LGRID_DUMMY_HEIGHT, skipTime, newYears)
{
    //ctor
    sprintf(message, "Booting up: Grid");

    int network = 0;
    int cnt=0;
    // TODO: his really needs changed, but for testing it is good enough
    for (int i = 0; i < LGRID_TOTAL_PIXALS; i++)
    {
        if (cnt==170)
        {
            cnt = 0;
            ++network;
            //printf("i: %d, CNT: %d, network: %d\n", i,cnt,network);
        }
        if (i == 2700)   // Force network swtich
        {
            cnt = 0;
            ++network;
            //printf("i: %d, CNT: %d, network: %d\n", i,cnt,network);
        }
        this->pixals[i] = net[network]->getRGB(cnt*3);
        ++cnt;
    }

    // Setup Dummy Pials
    for (int i = 0 ; i < (LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
    }

}

RGBLight *LargeGrid::getPixal(int x, int y)
{
    int xmod = x%2;
    int pos = 0;

    if (x < 0 || x >= LGRID_PIXAL_WIDTH)
    {
        throw "Illegal Value of X in getPixal";
    }

    if (y < 0 || y >= LGRID_PIXAL_HEIGHT)
    {
        throw "Illegal Value of Y in getPixal";
    }

    if (xmod == 0)
    {
        pos = x * LGRID_PIXAL_HEIGHT + y;
    }
    else
    {
        pos = x*LGRID_PIXAL_HEIGHT + (LGRID_PIXAL_HEIGHT-1) - y;
    }

    return this->pixals[pos];
}



RGBLight *LargeGrid::getBoard(int x, int y)
{
    int pos = y*LGRID_DUMMY_WIDTH + x;

    if (x <0 || x >= LGRID_DUMMY_WIDTH)
    {
        throw "Illegal Value for X in getBoard";
    }

    if (y <0 || y >= LGRID_DUMMY_HEIGHT)
    {
        throw "Illegal Value for Y in getBoard";
    }

// TODO (ghormann#1#): THis should be removed for production.  It isn't necessary.

    if (pos > (LGRID_DUMMY_HEIGHT * LGRID_DUMMY_WIDTH))
    {
        throw "Sign:getBoard: Invalid x and y positions";
    }

    return board[pos];
}


void LargeGrid::test()
{
    int i;

    while(0)
    {
        for (int i = 0; i < LGRID_PIXAL_WIDTH; i++)
        {
            int row = i%LGRID_PIXAL_HEIGHT;
            for (int j =0; j < LGRID_PIXAL_WIDTH; j++)
            {
                getPixal(j,row)->set(RGBColor::RED);
            }

            for (int j=0; j< LGRID_PIXAL_HEIGHT; j++)
            {
                getPixal(i,j)->set(RGBColor::GREEN);
            }

            gjhSleep(0.25);
            for (int j =0; j < LGRID_PIXAL_WIDTH; j++)
            {
                getPixal(j,row)->set(RGBColor::BLACK);
            }
            for (int j=0; j< LGRID_PIXAL_HEIGHT; j++)
            {
                getPixal(i,j)->set(RGBColor::BLACK);
            }
        }
    }

    /*
     * Test Animated Gif
     */
    while (1)
    {
        RGBPicture *pictures[4];
        int x, y, picWidth, picHeight;
        char filename[200];
        for (int i =0; i < 4; i++)
        {
            sprintf(filename,"/home/ghormann/Documents/src/gregslights/GregsLights/resources/toder_%d.png", i);
            pictures[i] = new RGBPicture(filename);
        }
        this->setDummyBackground(RGBColor::BLACK);
        while(1)    // because images don't destory correctly
        {
            for (int i =0; i < 4; i++)
            {
                pictures[i]->getSize(picWidth,picHeight);
                for (x = 0; x < picWidth; x++)
                {
                    for (y=0; y < picHeight; y++)
                    {
                        int r,g,b;
                        pictures[i]->getRGB(x,y,r,g,b);
                        this->getBoard(x,y)->set(r,g,b);
                    }
                }

                this->setDisplayPosition(0,0);
                gjhSleep(0.2);
            }
        }
    }

    /*
    * Test RGB Picture
    */
    while (1)
    {
        //
        // Scroll Left
        //
        int x, y, picWidth, picHeight;

        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/olaf64.png");
        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/test64.png");
        RGBPicture * picture = new RGBPicture("/home/ghormann/Documents/src/gregslights/GregsLights/resources/orig/test.png");
        picture->getSize(picWidth,picHeight);
        this->setDummyBackground(RGBColor::BLACK);
        for (x = 0; x < picWidth; x++)
        {
            for (y=0; y < picHeight; y++)
            {
                int r,g,b;
                picture->getRGB(x,y,r,g,b);
                this->getBoard(x+this->gridWidth,y)->set(r,g,b);
            }
        }

        this->setDisplayPosition(0,0);
        for (x = 0; x  < picWidth + this->gridWidth + 1; x++)
        {
            this->setDisplayPosition(x,0);
            gjhSleep(0.02);
        }
    }


    while (1)
    {
        //
        // Scroll up
        //
        int x, y, picWidth, picHeight;

        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/olaf64.png");
        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/test64.png");
        RGBPicture * picture = new RGBPicture("/home/ghormann/Documents/src/gregslights/GregsLights/resources/orig/test.png");
        picture->getSize(picWidth,picHeight);
        this->setDummyBackground(RGBColor::BLACK);
        for (x = 0; x < picWidth; x++)
        {
            for (y=0; y < picHeight; y++)
            {
                int r,g,b;
                picture->getRGB(x,y,r,g,b);
                this->getBoard(x,y+this->gridHeight)->set(r,g,b);
            }
        }

        this->setDisplayPosition(0,0);
        for (y = 0; y  < picHeight + this->gridHeight + 1; y++)
        {
            this->setDisplayPosition(0,y);
            gjhSleep(0.1);
        }
    }


    while(1)
    {
        sprintf(message, "Set Black");
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
        {
            this->pixals[i]->set(RGBColor::BLACK);
        }
        sleep(1);
        //sprintf(message, "Set fade Red 20, %d, %d", sizeof(Bulb), sizeof(FadeableBulb));
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(100,0,0,20);
            //this->pixals[i]->set(RGBColor::RED);
        }
        sleep(21);
        sprintf(message, "Set fade Black 0.25");
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,0.25);
            //this->pixals[i]->set(RGBColor::GREEN);
        }
        sleep(1);
        sprintf(message, "Set fade Green 1.25");
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,100,0,1.25);
            //this->pixals[i]->set(RGBColor::GREEN);
        }
        sleep(2);
        sprintf(message, "Set fade Black 10");
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,0,10);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(11);
        sprintf(message, "Set fade BLUE 2.0");
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
        {
            this->pixals[i]->fadeTo(0,0,100,2.0);
            //this->pixals[i]->set(RGBColor::BLUE);
        }
        sleep(2);
        sprintf(message, "Set fade BLACK 4");
        for (i = 0; i < LGRID_TOTAL_PIXALS; i++)
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
