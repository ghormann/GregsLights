#include "../include/Sign.h"
#include "../include/RGBPicture.h"
#include "../include/controller/DummyBulb.h"


Sign::Sign(bool skipTime, bool newYears, E131Network *net[]) : GenericGrid(SIGN_WIDTH,SIGN_HEIGHT,SIGN_DUMMY_WIDTH,SIGN_DUMMY_HEIGHT, skipTime, newYears)
{
    int cnt = 0;
    this->clear();
    this->generator = new MessageGenerator(this->timeInfo);
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = net[0]->getRGB(i*3); // port 1
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = net[1]->getRGB(i*3); // port 1
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = net[2]->getRGB(i*3); //port 1
    }
    for (int i = 0; i < 140; i++)
    {
        this->pixals[cnt++] = net[3]->getRGB(i*3); // port 1
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = net[4]->getRGB(i*3); //port 2
    }

    for (int i = 0; i < 140; i++)
    {
        this->pixals[cnt++] = net[5]->getRGB(i*3); //port 2
    }

    /* This should be 170 */
    for (int j = 0; j < 170; j++)
    {
        this->pixals[cnt++] = net[6]->getRGB(j*3); // port 3
    }

    /* This should be 170 */
    for (int j = 0; j < 170; j++)
    {
        this->pixals[cnt++] = net[7]->getRGB(j*3); // port 3
    }
    /* This should be 170 */
    for (int j = 0; j < 170; j++)
    {
        this->pixals[cnt++] = net[8]->getRGB(j*3); // port 3
    }
    /* This should be 170 */
    for (int j = 0; j < 170; j++)
    {
        this->pixals[cnt++] = net[9]->getRGB(j*3); // port 4
    }
    /* This should be 170 */
    for (int j = 0; j < 170; j++)
    {
        this->pixals[cnt++] = net[10]->getRGB(j*3); // port 4
    }
    /* This should be 170 */
    for (int j = 0; j < 170; j++)
    {
        this->pixals[cnt++] = net[11]->getRGB(j*3); // port 4
    }



    // Setup Dummy Pials
    for (int i = 0 ; i < (SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
    }
}

Sign::~Sign()
{
    //dtor
}

RGBLight * Sign::getPixal(int i)
{
    if (i < 0 || i >= TOTAL_SIGN_PIXALS)
    {
        throw "Sign::getPixal must be between 0 and TOTAL_SIGN_PIXALS";
    }
    return this->pixals[i];
}

RGBLight *Sign::getPixal(int x, int y)
{
    int xmod = x%2;
    int pos = 0;

    if (x < 0 || x >= SIGN_WIDTH)
    {
        throw "Illegal Value of X in getPixal";
    }

    if (y < 0 || y >= SIGN_HEIGHT)
    {
        throw "Illegal Value of Y in getPixal";
    }

    if (xmod == 0)
    {
        pos = x * SIGN_HEIGHT + y;
    }
    else
    {
        pos = x*SIGN_HEIGHT + (SIGN_HEIGHT-1) - y;
    }

    /*
    || FIx the mistakes that occured during wiring
    */
    if (x == 2 && y > 13)
    {
        pos +=1;
    }

    else if (x == 3)
    {
        if (y == 13)
        {
            pos -= 12;
        }
        else if (y > 13)
        {
            pos += 1;
        }

    }
    return getPixal(pos);
}

RGBLight *Sign::getBoard(int x, int y)
{
    int pos = y*SIGN_DUMMY_WIDTH + x;

    if (x <0 || x >= SIGN_DUMMY_WIDTH)
    {
        throw "Illegal Value for X in getBoard";
    }

    if (y <0 || y >= SIGN_DUMMY_HEIGHT)
    {
        throw "Illegal Value for Y in getBoard";
    }

    if (pos > (SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH))
    {
        throw "Sign:getBoard: Invalid x and y positions";
    }

    return board[pos];
}

void Sign::staticSecondsToGo(RGBColor *fgColor, RGBColor *bgColor)
{
    for (int i = 0; i < SIGN_WIDTH; i++)

    {
        for (int j = 0; j < SIGN_HEIGHT; j++)
        {
            getBoard(i,j)->set(bgColor);
        }
    }
    int x=7;
    int y=6;
    x +=drawLetterSmall('S', fgColor, x,y) +1;
    x +=drawLetterSmall('E', fgColor, x,y) +1;
    x +=drawLetterSmall('C', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;
    x +=drawLetterSmall('N', fgColor, x,y) +1;
    x +=drawLetterSmall('D', fgColor, x,y) +1;
    x +=drawLetterSmall('S', fgColor, x,y) +1;

    x+= 4;
    x +=drawLetterSmall('T', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;
    x+=4;
    x +=drawLetterSmall('G', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;

    setDisplayPosition(0,0);

}



void Sign::flashSecondsToGo(int times, double delay)
{
    RGBColor *bgColor = new RGBColor(0,0,5);
    for (int i =0; i < times; i++)
    {
        staticSecondsToGo(RGBColor::GREEN, bgColor);
        gjhSleep(delay);
        staticSecondsToGo(RGBColor::RED, bgColor);
        gjhSleep(delay);
        staticSecondsToGo(RGBColor::PURPLE, bgColor);
        gjhSleep(delay);
    }
    delete bgColor;

}




void Sign::rotateSecondsToGo()
{
    sprintf(message, "Flashing Seconds To Go");
    RGBColor *bgColor = new RGBColor(5,0,5);
    staticSecondsToGo(RGBColor::GREEN, bgColor);
    delete bgColor;

    sleep(3);

    bgColor = new RGBColor(0,5,0);
    staticSecondsToGo(RGBColor::RED, bgColor);
    delete bgColor;
    sleep(3);
    flashSecondsToGo(1,1.0);
    flashSecondsToGo(1,0.7);
    flashSecondsToGo(2,0.5);
    flashSecondsToGo(2,0.4);
    flashSecondsToGo(2,0.3);
    flashSecondsToGo(4,0.2);
    flashSecondsToGo(6,0.1);
    sleep(5);

    wipeToRight(RGBColor::BLACK, 0.05);
}

void Sign::run()
{
    double textSpeed = 0.04;

    // Not on douring the day
    if (timeInfo->isDayLight())
    {
        sprintf(message, "Sleeping During the day (%02d)",
                timeInfo->getHourOfDay());
        setDummyBackground(RGBColor::BLACK);
        setDisplayPosition(0,0);
        sleep(5);
        return;
    }

    //
    // Do what a sign should do
    //
    int numSeconds = timeInfo->getSecondsUntil();

    if (numSeconds < 33 && numSeconds > 0)
    {
        countdown();
    }
    else if (numSeconds > 32 && numSeconds  <80)
    {
        scrollText(RGBColor::WHITE, RGBColor::BLACK, "READY TO YELL?", textSpeed);
    }
    else
    {

        setDummyBackground(RGBColor::BLACK);
        setDisplayPosition(0,0);
        checkClear();

        int i = 9;
        while (useMap[i] == 1)
        {
            i = rand() % SIGN_OPTIONS;  // Don't show same message twice in a row.
        }
        useMap[i] = 1;

        switch(i)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            scrollText(RGBColor::getRandom(), RGBColor::BLACK, generator->getMessage(), textSpeed);
            break;
        case 5:
            if (numSeconds > 0)
                rotateSecondsToGo();
            break;
        case 6:
            fewTrees();
            break;
        case 7:
            snowballFight();
            break;
        case 8:
            scrollSanta();
            break;
        case 9:
            colors();
            break;
        }
    }
}

void Sign::checkClear()
{
    for (int i = 0; i < SIGN_OPTIONS; i++)
    {
        if (useMap[i] == 0) return;
    }
    // If we got this far, everything is used up.  Clear it.
    this->clear();
}


void Sign::clear()
{
    for (int i = 0; i < SIGN_OPTIONS; i++)
    {
        useMap[i] = 0;
    }

}



void Sign::fewTrees()
{
    sprintf(message, "A Few Trees");

    // Few Christimas Trees
    RGBColor *bgColor = new RGBColor(5,0,5);
    setDummyBackground(bgColor);
    int y=SIGN_HEIGHT+5;
    drawSpecial(0,2,GRID_TREE);
    drawSpecial(16,0, GRID_CANDY);
    drawSpecial(32,2,GRID_TREE);
    drawSpecial(0+SIGN_WIDTH/2,0, GRID_CANDY);
    drawSpecial(16+SIGN_WIDTH/2,2,GRID_TREE);
    drawSpecial(32+SIGN_WIDTH/2,0, GRID_CANDY);
    setDisplayPosition(0,y);
    while(y>=0)
    {
        setDisplayPosition(0,y--);
        gjhSleep(0.06);
    }
    delete bgColor;
    sleep(6);
    wipeDown(RGBColor::BLACK, 0.06);


}

void Sign::moveBall(int x, RGBColor *bgColor, int startY)
{
    int ballY = startY + (abs(24-x)/2)-1;
    if (ballY < startY)
        ballY = startY-1;

    getBoard(x+SNOWBALL_OFFSET,ballY)->set(RGBColor::WHITE);
    getBoard(x+1+SNOWBALL_OFFSET,ballY)->set(RGBColor::WHITE);
    getBoard(x+SNOWBALL_OFFSET,ballY+1)->set(RGBColor::WHITE);
    getBoard(x+1+SNOWBALL_OFFSET,ballY+1)->set(RGBColor::WHITE);
    redrawDisplay();
    gjhSleep(0.1);
    getBoard(x+SNOWBALL_OFFSET,ballY)->set(bgColor);
    getBoard(x+SNOWBALL_OFFSET,ballY+1)->set(bgColor);
    getBoard(x+1+SNOWBALL_OFFSET,ballY)->set(bgColor);
    getBoard(x+1+SNOWBALL_OFFSET,ballY+1)->set(bgColor);

}

void Sign::snowballFight()
{
    sprintf(message, "Snowball Fights");

    RGBColor *bgColor = new RGBColor(0,0,10);
    RGBColor *fgColor = RGBColor::PURPLE;
    setDummyBackground(bgColor);
    int y=0;
    int x=8+SNOWBALL_OFFSET;

    x +=drawLetterSmall('S', fgColor, x,y) +1;
    x +=drawLetterSmall('N', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;
    x +=drawLetterSmall('W', fgColor, x,y) +1;

    x=11+SNOWBALL_OFFSET;
    y=11;
    x +=drawLetterSmall('B', fgColor, x,y) +1;
    x +=drawLetterSmall('A', fgColor, x,y) +1;
    x +=drawLetterSmall('L', fgColor, x,y) +1;
    x +=drawLetterSmall('L', fgColor, x,y) +1;

    y=SIGN_HEIGHT+2;
    x=10+SNOWBALL_OFFSET;
    //fight
    x +=drawLetterSmall('F', fgColor, x,y) +1;
    x +=drawLetterSmall('I', fgColor, x,y) +1;
    x +=drawLetterSmall('G', fgColor, x,y) +1;
    x +=drawLetterSmall('H', fgColor, x,y) +1;
    x +=drawLetterSmall('T', fgColor, x,y) +1;
    setDisplayPosition(0,0);
    sleep(2);

    y=0;
    while (y < 13)
    {
        setDisplayPosition(0,y++);
        gjhSleep(0.1);
    }
    /*
     * No sleep. moveBall will sleep
     */
    setDisplayPosition(0,y++);


    /*
     * Now draw snowmen and scroll down!
     */
    int snowmanY = SIGN_HEIGHT+15;
    drawSpecial(0+SNOWBALL_OFFSET,snowmanY,GRID_SNOWMEN);
    drawSpecial(33+SNOWBALL_OFFSET,snowmanY,GRID_SNOWMEN_REVERSE);

    for (int x=15; x<33; x++)
    {
        moveBall(x, bgColor, snowmanY);
        setDisplayPosition(0,y++);
    }
    while (y <= snowmanY)
    {
        setDisplayPosition(0,y++);
        gjhSleep(0.1);
    }
    redrawDisplay();

    for (int i = 0; i < 2; i++)
    {
        for (int x=15; x<33; x++)
        {
            moveBall(x, bgColor,snowmanY);
        }
        redrawDisplay();
        sleep(1);

        for (int x=32; x>14; x--)
        {
            moveBall(x, bgColor, snowmanY);
        }

        redrawDisplay();
        sleep(1);
    }

    /*
     * Fade to black;
     */
    int duration = 2;
    for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
    {
        getPixal(i)->fadeTo(0,0,0,duration);
    }
    sleep(duration);
    delete bgColor;
}

void Sign::scrollSanta()
{
    sprintf(message, "Scroll Santa");
    RGBColor *bgColor = new RGBColor(0,0,15);
    setDummyBackground(bgColor);
    int xPos = SIGN_WIDTH+18;
    drawSpecial(xPos,8,GRID_SLEIGH);
    xPos += 15;
    for (int i = 0; i < 3; i++)
    {
        drawSpecial(xPos,5,GRID_DEER_2);
        xPos+= 15;
    }
    drawSpecial(xPos,5,GRID_DEER_2_RED);

    xPos += 40;
    setDisplayPosition(xPos,0);

    // Start Scrolling
    while (xPos > 0)
    {
        xPos--;
        if (xPos < SIGN_WIDTH+7)
        {
            setDummyBackground(bgColor, 0,0,SIGN_WIDTH+17, SIGN_HEIGHT);
            drawSpecial(xPos,4, GRID_PRESENT_RED);  // Present #1
        }
        if (xPos < SIGN_WIDTH-7)
        {
            drawSpecial(xPos+14,4, GRID_PRESENT_BLUE);  // Present #2
        }
        if (xPos < SIGN_WIDTH-14*2+7)
        {
            drawSpecial(xPos+14*2,4, GRID_PRESENT_GREEN);  // PResent #3
        }

        if (xPos < SIGN_WIDTH-14*3+7)
        {
            drawSpecial(xPos+14*3,4, GRID_PRESENT_PURPLE);  // PResent #3
        }

        if (xPos < SIGN_WIDTH-14*4+7)
        {
            drawSpecial(xPos+14*4,4, GRID_PRESENT_GREEN);  // PResent #3
        }

        if (xPos < SIGN_WIDTH-14*5+7)
        {
            drawSpecial(xPos+14*5,4, GRID_PRESENT_BLUE);  // PResent #3
        }

        if (xPos < SIGN_WIDTH-14*6+7)
        {
            drawSpecial(xPos+14*6,4, GRID_PRESENT_RED);  // PResent #3
        }


        setDisplayPosition(xPos, 0);
        gjhSleep(0.05);
    }
    sleep(3);
    delete bgColor;

}

void Sign::colors()
{
    sprintf(message, "Psychedelic Candy Cane");

    RGBColor *d[25];
    d[0]=d[1]=d[2]=d[3]=RGBColor::RED;
    d[4]=d[5]=d[6]=d[7]=RGBColor::GREEN;
    d[8]=d[9]=d[10]=d[11]=RGBColor::BLUE;
    d[12]=d[13]=d[14]=d[15]=RGBColor::PURPLE;
    d[16]=d[17]=d[18]=d[19]=RGBColor::ORANGE;

    int i = 150;

    while(--i > 0)
    {
        for (int x = 0; x < SIGN_WIDTH; x++)
        {
            for (int y = 0; y < SIGN_HEIGHT; y++)
            {
                int distance = gjhDistance(SIGN_WIDTH/2, SIGN_HEIGHT/2, x, y);
                RGBColor *color = d[(distance+i)%20];
                getBoard(x,y)->set(color);
            }
        }
        drawSpecial(SIGN_WIDTH/2-7,0,GRID_CANDY);
        setDisplayPosition(0,0);
        gjhSleep(0.05);
    }
    for (int x=0; x<SIGN_WIDTH; x++)
    {
        for (int y=0; y<SIGN_HEIGHT; y++)
        {
            this->getPixal(x,y)->fadeTo(0,0,0,1);
        }
    }
    sleep(1);
}

void Sign::test()
{
    while(1)
    {
        run();
    }

    /*
     * Test RGB Picture
     */
    while (1)
    {
        int x, y, width, height;

        RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/olaf64.png");
        //RGBPicture * picture = new RGBPicture("/home/ghormann/Downloads/sleigh32.png");
        picture->getSize(width,height);
        this->setDummyBackground(RGBColor::BLACK);
        for (x = 0; x < width; x++)
        {
            for (y=0; y < height; y++)
            {
                int r,g,b;
                picture->getRGB(x,y,r,g,b);
                this->getBoard(x,y+SIGN_HEIGHT)->set(r,g,b);
            }
        }

        this->setDisplayPosition(0,0);
        for (y = 0; y  < height + SIGN_HEIGHT + 1; y++)
        {
            this->setDisplayPosition(0,y);
            gjhSleep(0.1);
        }
    }

    while(0)
    {
        getPixal(10,10)->fadeTo(100,0,0,10.0);
        sleep(12);

        getPixal(10,10)->set(RGBColor::GREEN);
        sleep(2);

        getPixal(10,10)->fadeTo(100,100,100,10.0);
        sleep(5);
        getPixal(10,10)->set(0,0,100);
        sleep(1);

    }

    while (1)
    {
        //timeInfo->setSkipTimeCheck(true);


        /*
                RGBColor *bgColor = new RGBColor(0,0,15);
                setDummyBackground(bgColor);
                drawSpecial(0,5,SIGN_SLEIGH);
                drawSpecial(15,0,SIGN_DEER_2);
                drawSpecial(30,0,SIGN_DEER_2_RED);
                setDisplayPosition(0,0);
                sleep(60);
                delete bgColor;
        */

        colors();
        //flashSecondsToGo(4,1);
        //rotateSecondsToGo();
        //this->countdown();
        //this->fewTrees();
        //snowballFight();
        //scrollText(RGBColor::getRandom(), RGBColor::BLACK, generator->getMessage(), 0.04);
        //scrollText(RGBColor::PURPLE, RGBColor::BLACK, "MADE YOUR RESOLUTIONS FOR THE NEW YEAR?", 0.04);
        //run();
    }

    int betweenPixals = 2500;
    int betweenColors = 3;
    this->pixals[0]->set(100,100,100);
    sleep(1);
    this->pixals[0]->set(RGBColor::BLACK);


    while (true)
    {
        setDummyBackground(RGBColor::BLACK);
        setDisplayPosition(0,0);



        for (int x = 0; x < SIGN_WIDTH; x++)
        {
            for (int y = 0; y < SIGN_HEIGHT; y++)
            {
                getPixal(x,y)->set(100,0,0);
                usleep(betweenPixals);
            }
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(100,0,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(0,100,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(0,0,100);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(100,100,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,100,0,100,0,0,betweenColors);
        }
        sleep(betweenColors*2);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,0,0,0,100,0,betweenColors);
        }
        sleep(betweenColors*2);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(0,100,0,0,0,100,betweenColors);
        }
        sleep(betweenColors*2);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(0,0,100,100,100,100,betweenColors);
        }
        sleep(betweenColors*3);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,100,100,0,0,0,betweenColors);
        }
        sleep(betweenColors);
        sleep(2);
    }
}
