#include "../include/StarField.h"
#include "../include/controller/FadeableBulb.h"
#include <iostream>

using namespace std;

StarField::StarField(E131Network *net, TimeInfo *tmInfo)
{
    this->timeInfo = tmInfo;
    //ctor
    // Start at DMX 100, but really 99 in E131
    for (int i = 0; i < STAR_COUNT+1; ++i)
    {
        stars[i] = net->getRGB(99 + i*3);
    }


    // Fix it;
    swapStars(8,0); //OK
    swapStars(7,1); //OK
    swapStars(6,2); //OK
    swapStars(8,3); //OK
    swapStars(7,4); //OK
    // 5 no change
    swapStars(17,6); //OK
    // 7 ok
    // 7 -- Row 2
    swapStars(17,8); //OK #8
    swapStars(17,9); //OK #9
    swapStars(16,10); //Ok
    swapStars(13,11); //OK
    //12 ok
    swapStars(16,13); //OK
    swapStars(17,14);
    swapStars(16,15);
    swapStars(17,16);
    //17 ok
    swapStars(38,18); //OK
    swapStars(38,19); //OK
    swapStars(38,20); //OK
    swapStars(26,21); //OK
    swapStars(24,22); //OK
    swapStars(24,23); //OK
    //24 ok
    swapStars(28,25); //ok
    swapStars(29,26); //ok
    swapStars(33,27);
    //28 ok
    //29 ok
    swapStars(40,30); //ok
    swapStars(38,31); //ok
    swapStars(35,32); //ok
    //33 ok
    swapStars(38,34); //ok
    swapStars(37,35); //ok
    swapStars(39,36); //ok
    swapStars(44,37); //ok
    //38 ok
    swapStars(44,39); //ok
    //40 ok
    swapStars(44,41); //ok


    // RED, GREEN, BLUE, WHITE, DK RED, PURPLE, DK BLUE, YELOW, DARK GREEN

    startPos[0] = 'L';
    startPos[1] = 'L';
    startPos[2] = 'L';
    startPos[3] = 'R';
    startPos[4] = 'R';
    startPos[5] = 'R';
    startPos[6] = 'R';
    startPos[7] = 'L';
    startPos[8] = 'L';
    startPos[9] = 'L';
    startPos[10] = 'R';
    startPos[11] = 'R';
    startPos[12] = 'R';
    startPos[13] = 'L';
    startPos[14] = 'L';
    startPos[15] = 'L';
    startPos[16] = 'R';
    startPos[17] = 'R';
    startPos[18] = 'R';
    startPos[19] = 'L';
    startPos[20] = 'L';
    startPos[21] = 'L';
    startPos[22] = 'R';
    startPos[23] = 'R';
    startPos[24] = 'R';
    startPos[25] = 'L';
    startPos[27] = 'L';
    startPos[28] = 'L';
    startPos[29] = 'R';
    startPos[30] = 'R';
    startPos[31] = 'R';
    startPos[32] = 'R';
    startPos[33] = 'L';
    startPos[34] = 'L';
    startPos[35] = 'L';
    startPos[37] = 'R';
    startPos[38] = 'R';
    startPos[39] = 'R';
    startPos[40] = 'L';
    startPos[41] = 'L';
    startPos[42] = 'R';
    startPos[43] = 'R';
    startPos[44] = 'R';

}

StarField::~StarField()
{
    //dtor
}

void StarField::fadeAllTo(int r, int g, int b, double duration) {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i]->fadeTo(r,g,b,duration);
    }

}


void StarField::fadeAllTo(RGBColor *c, double duration) {
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i]->fadeTo(c, duration);
    }
}

void StarField::setLeft(RGBColor *c)
{
    for (int i =0; i < STAR_COUNT; i++)
    {
        if ('L' == startPos[i])
            stars[i]->set(c);
    }
}

void StarField::setRight(RGBColor *c)
{
    for (int i =0; i < STAR_COUNT; i++)
    {
        if ('R' == startPos[i])
            stars[i]->set(c);
    }
}


void StarField::swapStars(int i, int j)
{
    RGBLight *temp = stars[i];
    stars[i] = stars[j];
    stars[j] = temp;
}

RGBLight* StarField::getStar(int id)
{
    if (id < 0 || id >= STAR_COUNT)
    {
        cout << "Invalid Star ID" << endl;
        throw "Invalid Star ID";
    }
    return stars[id];
}

void StarField::setAll(RGBColor *c)
{
    for (int i=0; i < STAR_COUNT; i++)
    {
        stars[i]->set(c);
    }
}


void StarField::test()
{
    //((FadeableBulb *)stars[9]->getRedBulb())->setDebug(true);
    double duration = 5.0;
    RGBColor *colors[4];
    colors[0] = RGBColor::RED;
    colors[1] = RGBColor::GREEN;
    colors[2] = RGBColor::BLUE;
    colors[3] = RGBColor::WHITE;

    while(0)
    {
        int i ;
        for (i =38; i < STAR_COUNT; i++)
        {
            stars[i]->set(5,5,5);
            gjhSleep(0.5);
        }
        i = 38;
        stars[i+0]->set(RGBColor::RED);
        stars[i+1]->set(RGBColor::GREEN);
        stars[i+2]->set(RGBColor::BLUE);
        stars[i+3]->set(RGBColor::WHITE);
        stars[i+4]->set(2,0,0);
        stars[i+5]->set(RGBColor::PURPLE);
        //stars[i+6]->set(0,0,2);
        //stars[i+7]->set(RGBColor::YELLOW);
        //stars[i+8]->set(0,2,0);

        sleep(10);

    }
    while(1)
    {
        for (int i =0; i<2; i++)
        {
            for (int j=0; j < STAR_COUNT; j++)
            {
                int c = (i+j)%4;
                stars[j]->fadeTo(colors[c], duration);
            }

            sleep(duration);
        }

        for (int j = 0; j < STAR_COUNT; j++)
        {
            stars[j]->fadeTo(RGBColor::BLACK, duration);
        }
        sleep(duration);
    }
}
