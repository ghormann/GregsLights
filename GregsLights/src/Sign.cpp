#include "../include/Sign.h"
#include <unistd.h>
#include "../include/controller/DummyBulb.h"
#include <stdio.h>

Sign::Sign(E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4, E131Network *n5, E131Network *n6)
{
    currentX = 0;
    currentY = 0;
    int cnt = 0;
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n1->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n2->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n3->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n4->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n5->getRGB(i*3);
    }
    for (int i = 0; i < 110; i++)
    {
        this->pixals[cnt++] = n6->getRGB(i*3);
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
    return getPixal(y*SIGN_WIDTH + x);
}

RGBLight *Sign::getBoard(int x, int y)
{
    int pos = y*SIGN_DUMMY_WIDTH + x;
    if (pos > (SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH))
    {
        throw "Sign:getBoard: Invalid x and y positions";
    }

    return board[pos];
}

void Sign::scrollSecondsUntil()
{
    for (int i = 0; i < SIGN_DUMMY_WIDTH; i++)
    {
        for (int j = 0; j < SIGN_DUMMY_HEIGHT; j++)
        {
            getBoard(i,j)->setStdColor(GREEN);
        }
    }

    drawLetter('M', RED, 0,0);
    setDisplayPosition(0,0);
    sleep(5);

    for (int i = 0; i < 40; i++)
    {
        setDisplayPosition(i,0);
        sleep(1);
    }
}

int Sign::drawLetter(char letter, RGB_COLOR color, int startX, int startY)
{
    int x;
    int y;
    if (letter == 'M')
    {
        x =  startX;
        for (y = startY+2; y < startY + 14; y++)
        {
            getBoard(x,y)->setStdColor(color);
        }
        getBoard(++x,startY+3)->setStdColor(color);
        getBoard(++x,startY+4)->setStdColor(color);
        getBoard(++x,startY+5)->setStdColor(color);
        getBoard(++x,startY+6)->setStdColor(color);

        getBoard(++x,startY+7)->setStdColor(color);

        getBoard(++x,startY+6)->setStdColor(color);
        getBoard(++x,startY+5)->setStdColor(color);
        getBoard(++x,startY+4)->setStdColor(color);
        getBoard(++x,startY+3)->setStdColor(color);

        ++x;
        for (y = startY+2; y < startY + 14; y++)
        {
            getBoard(x,y)->setStdColor(color);
        }

        return x+1;
    }
    else
    {
        return x;
    }
}

void Sign::redrawDisplay()
{
    this->setDisplayPosition(currentX, currentY);
}

void Sign::setDisplayPosition(int xOffset, int yOffset)
{
    for (int i =0; i < SIGN_WIDTH; i++)
    {
        for (int j = 0; j < SIGN_HEIGHT; j++)
        {
            this->getPixal(i,j)->copyFrom(this->getBoard(i+xOffset,j+yOffset));
        }
    }
}

void Sign::test()
{
    int betweenPixals = 20000;
    int betweenColors = 2;
    scrollSecondsUntil();
    while (true)
    {
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
            this->pixals[i]->set(100,100,100);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,100,100,100,0,0,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,0,0,0,100,0,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(0,100,0,0,0,100,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(0,0,100,0,0,0,betweenColors);
        }
        sleep(betweenColors);
        sleep(2);
    }
}
