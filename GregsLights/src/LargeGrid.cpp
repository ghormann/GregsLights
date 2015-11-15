#include "../include/LargeGrid.h"
#include <stdio.h>
#include <string.h>
#include "../include/controller/DummyBulb.h"
#include "RGBPicture.h"
#include <vector>
#include <iostream>

using namespace std;


LargeGrid::LargeGrid(bool skipTime, bool newYears,E131Network *net[], E131Network *net2[]) : GenericGrid(LGRID_PIXAL_WIDTH,LGRID_PIXAL_HEIGHT,LGRID_DUMMY_WIDTH,LGRID_DUMMY_HEIGHT, skipTime, newYears)
{
    //ctor
    sprintf(message, "Booting up: Grid");

    int network = 0;
    int x = 0;
    int y = LGRID_PIXAL_HEIGHT -1;
    int networkPixal = 0;
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
        if (i == 650 || i == 650*2 || i == 650*3 )
        {
            //printf("CHANGE 1\n");
            ++network;
            networkPixal = 0;
        }
        else if (networkPixal == 170)
        {
            //printf("CHANGE 2\n");
            networkPixal = 0;
            ++network;
        }

        //printf("x: %d, y: %d, network: %d, networkPixal: %d, i: %d\n", x,y,network,networkPixal, i);
        this->pixals[x * LGRID_PIXAL_HEIGHT + y] = net[network]->getRGB(networkPixal*3);
        y += dir;
        ++networkPixal;
    }

    /*
     * Do the last Controller backwards
     */
    x= LGRID_PIXAL_WIDTH -1;
    y= LGRID_PIXAL_HEIGHT-1;
    dir = -1;
    network = 0;
    networkPixal = 0;
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
        if (counter == 651 || counter == 650*2+1 || counter == 650*3+1 )
        {
            //printf("CHANGE A, %d\n", counter);
            ++network;
            networkPixal = 0;
        }
        else if (networkPixal == 170)
        {
            //printf("CHANGE B\n");
            networkPixal = 0;
            ++network;
        }
        //printf("x: %d, y: %d, network: %d, networkPixal: %d - BACKWARDS\n", x,y,network,networkPixal);
        this->pixals[x * LGRID_PIXAL_HEIGHT + y] = net2[network]->getRGB(networkPixal*3);
        y += dir;
        ++networkPixal;

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

    //Roate Colors
    while(0)
    {
        for (int x = 0; x < LGRID_PIXAL_WIDTH; x++)
        {
            for (int y = 0; y < LGRID_PIXAL_HEIGHT; y++)
            {
                this->getPixal(x,y)->set(25,0,0);
            }
        }

        gjhSleep(1.5);

        for (int x = 0; x < LGRID_PIXAL_WIDTH; x++)
        {
            for (int y = 0; y < LGRID_PIXAL_HEIGHT; y++)
            {
                this->getPixal(x,y)->set(0,25,0);
            }
        }

        gjhSleep(1.5);
    }

    // Make onne line white
    while(0)
    {
        int x = 80;
        for (int y = 0; y < LGRID_PIXAL_HEIGHT; y++)
        {
            this->getPixal(x,y)->set(RGBColor::WHITE);
            //gjhSleep(1.0);

        }
        gjhSleep(2.0);
        for(int y = 0; y< LGRID_PIXAL_HEIGHT; y++)
        {
            this->getPixal(x,y)->set(RGBColor::BLACK);
        }
    }

    // show lines
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

    while(0)
    {
        spiral(RGBColor::RED);
        spiral(RGBColor::BLUE);
        RGBPicture *p = RGBPicture::getPicture( string("yellow_start_64.png"));
        this->colorAroundPicture(p,150);
    }

    // Show all Pictures
    while(0)
    {
        std::vector<RGBPicture> allPictures = RGBPicture::getAllPictures();
        vector<RGBPicture>::iterator it;
        cout << "All Pictures: "  << allPictures.size() << endl;


        for(it = allPictures.begin(); it != allPictures.end(); it++)
        {
            RGBPicture pict = (*it);
            cout << pict.getName() << endl;
            this->setBackground(RGBColor::BLACK);
            this->showPictureNow(pict,0,0,false);
            gjhSleep(1.5);
        }
    }

    /*
     * Test Animated Gif
     */
    while (0)
    {
        string name = string("toder");
        showMovie(name,6,0.15,0,0);
        spiral(RGBColor::BLACK);

    }

    // static picture
    while(0)
    {
        int x, y, picWidth, picHeight;
        RGBPicture * picture = new RGBPicture("/home/ghormann/Documents/src/gregslights/GregsLights/resources/homer46.png");
        picture->getSize(picWidth,picHeight);
        this->setDummyBackground(RGBColor::BLACK);
        for (x = 0; x < picWidth; x++)
        {
            for (y=0; y < picHeight; y++)
            {
                int r,g,b;
                picture->getRGB(x,y,r,g,b);
                this->getBoard(x,y)->set(r,g,b);
            }
        }

        this->setDisplayPosition(0,0);
        sleep(5);
    }

    /*
    * Test RGB Picture (Scroll Left)
    */
    while (1)
    {
        //
        // Scroll Left
        //
        RGBPicture *left = RGBPicture::getPicture(("train_blue_46.png"));
        RGBPicture *right = RGBPicture::getPicture(("train_right_46.png"));
        scrollPictureLeft(*left,0.01,false);
        scrollPictureRight(*right,0.01,false);
    }

    // Scroll up RGB Picture
    while (1)
    {
        //
        // Scroll up
        //
        int x, y, picWidth, picHeight;

        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/olaf64.png");
        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/test64.png");
        RGBPicture * picture = new RGBPicture("/home/ghormann/Documents/src/gregslights/GregsLights/resources/olaf64.png");
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
