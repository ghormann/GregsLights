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
    timeinfo = new TimeInfo(skipTime,newYears);

    int network = 0;
    int x = 0;
    int y = LGRID_PIXAL_HEIGHT -1;
    int networkPixal = 0;
    nextAction = LG_SLEEP;
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

    // make sure Pictures are Loaded
    RGBPicture::getAllPictures();
}

void LargeGrid::setNextAction(GRID_ACTIONS a)
{
    this->nextAction = a;
    this->interruptThread();
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

void LargeGrid::scrollGrinch()
{
    setDummyBackground(RGBColor::BLACK);
    RGBPicture* pic = RGBPicture::getPicture(("Grinch_only_92.png"));
    showPictureDummy(*pic, 60, 0,true);
    int y=0;
    writeTextSmall(RGBColor::WHITE,0,y,("MAYBE"));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("CHRISTMAS"));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("HE THOUGHT"));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("DOESN'T "));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("COME FROM"));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("A STORE."));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("CHRISTMAS"));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("PERHAPS..."));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("IT MEANS A LITTLE"));
    y+=12;
    writeTextSmall(RGBColor::WHITE,0,y,("BIT MORE!"));

    setDisplayPosition(0,0);
    sleep(1);
    int stop = y-gridHeight+12;
    for (int y = 0; y < stop; y++)
    {
        setDisplayPosition(0,y);
        gridSleep(0.3);
    }
    for (int i = 0; i < 5; i++)
    {
        gridSleep(i);
    }

}

void LargeGrid::peakSanta()
{
    RGBPicture *pic = RGBPicture::getPicture(("santa-hat_92.png"));
    double delay = 0.1;

    int y=0, picWidth, picHeight;
    pic->getSize(picWidth,picHeight);
    int x = (gridWidth - picWidth)/2; // Center pic
    x = x < 0 ? 0 : x;

    this->setDummyBackground(RGBColor::BLACK,0,0,gridWidth,picHeight *2 + gridHeight);
    showPictureDummy(*pic,x,gridHeight,false);

    writeTextSmall(RGBColor::WHITE, 0,0+10, "HE KNOWS IF YOU");
    writeTextSmall(RGBColor::WHITE, 0,14+10, "  HAVE BEEN BAD");
    writeTextSmall(RGBColor::WHITE, 0,28+10, "     OR GOOD. ");

    this->setDisplayPosition(0,10);
    gridSleep(2.5);


    y=10;
    while (y < 58)
    {
        this->setDisplayPosition(0,++y);
        gridSleep(delay);
    }

    setDummyBackground(RGBColor::BLACK,0,0,gridWidth,y-2);

    gridSleep(1.5);

    while (y >= 10)
    {
        this->setDisplayPosition(0,--y);
        gridSleep(delay);
    }
}

void LargeGrid::trainText(string left)
{
    RGBPicture *tleft = RGBPicture::getPicture(("train_blue_46.png"));

    double speed = 0.02;
    int x = 0;
    int y = 0;
    int picHeight, picWidth;

    setDummyBackground(RGBColor::BLACK);

    tleft->getSize(picWidth,picHeight);
    x = picWidth;
    y = (picHeight > gridHeight ? 0  : (gridHeight-picHeight)/2);

    this->showPictureDummy(*tleft,x,y,false);
    x=picWidth * 2 + 3;
    y=gridHeight/2;

    x =writeText(RGBColor::BLUE,x,y,left);
    x +=3;
    for (int i =0; i < x; i++)
    {
        setDisplayPosition(i,0);
        gridSleep(speed);
    }
}

void LargeGrid::rotatePictures()
{
    std::vector<RGBPicture> allPictures = RGBPicture::getAllPictures();
    while (1) // Runs forever, but is interruptable
    {

        while (picPos < allPictures.size())
        {
            RGBPicture pict = allPictures.at(picPos++);
            if (! pict.isMovie())
            {
                cout << pict.getName() << endl;
                this->setBackground(RGBColor::BLACK);
                this->showPictureNowCenter(pict,false);
                gridSleep(1.5);
            }
        }
        picPos = 0; // Reset
    }

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

        gjhSleep(1);

        for (int x = 0; x < LGRID_PIXAL_WIDTH; x++)
        {
            for (int y = 0; y < LGRID_PIXAL_HEIGHT; y++)
            {
                this->getPixal(x,y)->set(0,25,0);
            }
        }

        gjhSleep(1);
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

    while(1)
    {
        trainText("MERRY CHRISTMAS FROM THE HORMANN FAMILY");
    }

    // Start of Main.
    while(1)
    {

        /*
         * HAPPINES IS CATCHING SNOWFLAKES ON YOUR TONGUE
         * YOU'LL SHOOT YOUR EYE OUT KID.
         * https://darrellcreswell.files.wordpress.com/2012/12/joy-to-the-world-the-lord-has-come-luke-2-11.jpg?w=700
        */

        RGBPicture *p = RGBPicture::getPicture( string("yellow_start_64.png"));
        this->colorAroundPicture(p,150);


        //scrollText(RGBColor::WHITE, RGBColor::DARKGREEN,"SOME PEOPLE ARE WORTH MELTING FOR", 0.02);
        setDummyBackground(RGBColor::DARKGREEN, 0,0,gridWidth,gridHeight);
        writeTextSmall(RGBColor::WHITE, 0,0, "SOME PEOPLE ARE");
        writeTextSmall(RGBColor::WHITE, 0,14, "WORTH MELTING");
        writeTextSmall(RGBColor::WHITE, 33,28, "FOR ...");

        setDisplayPosition(0,0);
        gridSleep(4.5);
        wipeDown(RGBColor::BLACK,0.05);
        string olafMelt = string("olaf_melt_64");
        this->showMovie(olafMelt,3,0.15,0,0);
        //scrollText(RGBColor::RED, RGBColor::DARKGREEN,"IS YOUR HOUSE ON FIRE CLARK? .... NO AUNT BETHANY, THOSE ARE THE CHRISTMAS LIGHTS.....", 0.02);
        peakSanta();

        trainText("MERRY CHRISTMAS FROM THE HORMANN FAMILY");

        string name = string("toder");
        showMovieCenter(name,6,0.15);
        spiral(RGBColor::BLACK);

        string name1 = string("tongue");
        showMovieCenter(name1,3,0.15);
        spiral(RGBColor::BLACK);

        string name2 = string("scarf");
        showMovieCenter(name2,1,0.15);
        spiral(RGBColor::BLACK);

        string name3 = string("puss_boots");
        showMovieCenter(name3,6,0.15);
        spiral(RGBColor::BLACK);

        // Done above
        //string name4 = string("olaf_melt");
        //showMovieCenter(name4,3,0.15);
        //spiral(RGBColor::BLACK);

        string name5 = string("candles");
        showMovieCenter(name5,12,0.15);
        spiral(RGBColor::BLACK);

        string name6 = string("19237_64");
        showMovieCenter(name6,1,0.50);
        gridSleep(1);

        spiral(RGBColor::BLACK);

        candyCane();

        RGBPicture *pic = RGBPicture::getPicture(("olaf64.png"));
        scrollPictureUp(*pic,0.1,false);



        spiral(RGBColor::BLACK);
        spiral(RGBColor::PURPLE);
        sleep(1);
        spiral(RGBColor::BLACK);
        spiral(RGBColor::UNKNOWN);
        sleep(1);
        RGBPicture *p2 = RGBPicture::getPicture( string("christmas-tree_64_0.png"));
        this->colorAroundPicture(p2,150);
        scrollGrinch();


        std::vector<RGBPicture> allPictures = RGBPicture::getAllPictures();
        vector<RGBPicture>::iterator it;
        cout << "All Pictures: "  << allPictures.size() << endl;


        for(it = allPictures.begin(); it != allPictures.end(); it++)
        {
            RGBPicture pict = (*it);
            if (! pict.isMovie())
            {
                this->setBackground(RGBColor::BLACK);
                this->showPictureNowCenter(pict,false);
                gjhSleep(1.5);
            }
        }


        setDummyBackground(RGBColor::RED,0,0,gridWidth,gridHeight);
        writeTextSmall(RGBColor::WHITE,0,0, ("MERRY"));
        writeTextSmall(RGBColor::WHITE,0,16, ("CHRISTMAS"));
        writeTextSmall(RGBColor::WHITE,0,32, ("TO EVERYONE"));

        setDisplayPosition(0,0);
        sleep(5);
    }

    // Show all Pictures
    while(1)
    {

        spiral(RGBColor::RED);
        spiral(RGBColor::BLUE);
        spiral(RGBColor::PURPLE);
        spiral(RGBColor::ORANGE);
        spiral(RGBColor::BLACK);




        std::vector<RGBPicture> allPictures = RGBPicture::getAllPictures();
        vector<RGBPicture>::iterator it;
        cout << "All Pictures: "  << allPictures.size() << endl;


        for(it = allPictures.begin(); it != allPictures.end(); it++)
        {
            RGBPicture pict = (*it);
            if (! pict.isMovie())
            {
                cout << pict.getName() << endl;
                this->setBackground(RGBColor::BLACK);
                this->showPictureNowCenter(pict,false);
                gjhSleep(1.5);
            }
        }

    }

    /*
     * Test Animated Gif
     */
    while (0)
    {
        string name = string("tongue");
        showMovieCenter(name,6,0.15);
        spiral(RGBColor::BLACK);

    }

    // static picture
    while(0)
    {
        int x, y, picWidth, picHeight;
        RGBPicture * picture = new RGBPicture("/home/ghormann/Documents/src/gregslights/GregsLights/resources/test/xmas_tree_64_0.png");
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

    while(0)
    {
        spiral(RGBColor::RED);
        setDummyBackground(RGBColor::RED,0,0,gridWidth,gridHeight);
        writeTextSmall(RGBColor::WHITE,0,0, ("MERRY"));
        writeTextSmall(RGBColor::WHITE,0,16, ("CHRISTMAS"));
        writeTextSmall(RGBColor::WHITE,0,32, ("ALL"));

        setDisplayPosition(0,0);
        sleep(5);
    }

    /*
    * Test RGB Picture (Scroll Left)
    */
    while (0)
    {
        //
        // Scroll Left / right
        //
        RGBPicture *left = RGBPicture::getPicture(("train_blue_46.png"));
        RGBPicture *right = RGBPicture::getPicture(("train_right_46.png"));
        scrollPictureLeft(*left,0.01,false);
        scrollPictureRight(*right,0.01,false);
    }

    // Scroll up RGB Picture
    while (1)
    {
        RGBPicture *pic = RGBPicture::getPicture(("santa-hat_92.png"));
        scrollPictureUp(*pic,0.1,false);
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
    while (! timeinfo->isDisplayHours())
    {
        sprintf(message, "Sleeping during day (%02d)",
                timeinfo->getHourOfDay());
        setBackground(RGBColor::BLACK);
        gridSleep(5);;
    }
    string clark = ("clark_Plug");


    switch(nextAction)
    {
    case LG_CLARK:
        showMovieCenter(clark,1,0.1);
        break;
    case LG_GRINCH:
        scrollGrinch();
        break;
    case LG_HAT:
        peakSanta();
        gridSleep(5);
        break;
    case LG_SHOW_PICT:
        rotatePictures();
        break;
    default:
        cout << "Grid Sleep" << endl;
        setBackground(RGBColor::BLACK);
        gridSleep(5.0); // Interruptable
        break;
    }

}

LargeGrid::~LargeGrid()
{
    //dtor
}
