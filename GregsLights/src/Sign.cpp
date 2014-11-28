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

    int pos = drawLetter('M', RED, 0,0);
    pos = drawLetter('E', RED, pos+2,0);
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
    int x=0;
    int y=0;
    int offset = 0;
    char d[20][40];
    for (x = 0; x < 20; x++)
    {
        for (y=0; y<40; y++)
        {
            d[x][y]='0';
        }
    }
    if (letter == 'E')
    {
        for (x=0; x < 12; x++)
        {
            d[x][0] = d[x][13]='1';
        }
        for (y=0; y<14; y++)
        {
            d[2][y] = d[3][y] = '1';
        }
        d[10][1]=d[11][1]=d[11][2] = '1';
        d[12][10]=d[11][11]=d[11][12]=d[10][12]='1';
        for (x=2; x<10; x++)
            d[x][6] = '1';

        for (y=4; y<9; y++)
            d[9][y] = '1';

        offset = 12;
    }
    else if (letter == 'M')
    {
        y=-1;
        d[0][++y] = d[1][y] = d[2][y] = d[3][y]= d[4][y] =d[14][y]=d[15][y]=d[16][y]=d[17][y] = '1';
        d[2][++y]=d[3][y]=d[4][y] = d[13][y] = d[14][y] = d[15][y] = '1'; //1
        d[2][++y]=d[3][y]=d[4][y] =d[5][y]= d[13][y] = d[14][y] = d[15][y] = '1'; //2
        d[2][++y]=d[4][y]=d[5][y]=d[12][y]=d[14][y]=d[15][y]='1'; //3
        d[2][++y]=d[4][y]=d[5][y]=d[12][y]=d[14][y]=d[15][y]='1'; //4
        d[2][++y]=d[4][y]=d[5][y]=d[6][y]=d[12][y]=d[14][y]=d[15][y]='1'; //5
        d[2][++y]=d[5][y]=d[6][y]=d[11][y]=d[14][y]=d[15][y]='1'; //6
        d[2][++y]=d[5][y]=d[6][y]=d[7][y]=d[11][y]=d[14][y]=d[15][y]='1'; //7
        d[2][++y]=d[6][y]=d[7][y]=d[10][y]=d[14][y]=d[15][y]='1'; //8
        d[2][++y]=d[6][y]=d[7][y]=d[10][y]=d[14][y]=d[15][y]='1'; //9
        d[2][++y]=d[7][y]=d[8][y]=d[9][y]=d[14][y]=d[15][y]='1'; //10
        d[2][++y]=d[7][y]=d[8][y]=d[9][y]=d[14][y]=d[15][y]='1'; //11
        d[2][++y]=d[7][y]=d[8][y]=d[14][y]=d[15][y]='1'; //12
        d[0][++y] = d[1][y] = d[2][y] = d[3][y]= d[4][y] =d[12][y]=d[13][y]=d[14][y]=d[15][y]=d[16][y]=d[17][y] = '1'; //13

        offset=17;
    }

    for (x = 0; x < 20; x++)
    {
        for (y=0; y<40; y++)
        {
            if (d[x][y] == '1')
                this->getBoard(startX+x, startY+y+2)->setStdColor(color);
        }
    }

    return offset;

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
