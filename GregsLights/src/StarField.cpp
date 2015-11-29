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

    starPos[0] = 'L';
    starPos[1] = 'L';
    starPos[2] = 'L';
    starPos[3] = 'R';
    starPos[4] = 'R';
    starPos[5] = 'R';
    starPos[6] = 'R';
    starPos[7] = 'L';
    starPos[8] = 'L';
    starPos[9] = 'L';
    starPos[10] = 'R';
    starPos[11] = 'R';
    starPos[12] = 'R';
    starPos[13] = 'L';
    starPos[14] = 'L';
    starPos[15] = 'L';
    starPos[16] = 'R';
    starPos[17] = 'R';
    starPos[18] = 'R';
    starPos[19] = 'L';
    starPos[20] = 'L';
    starPos[21] = 'L';
    starPos[22] = 'R';
    starPos[23] = 'R';
    starPos[24] = 'R';
    starPos[25] = 'L';
    starPos[26] = 'L';
    starPos[27] = 'L';
    starPos[28] = 'L';
    starPos[29] = 'R';
    starPos[30] = 'R';
    starPos[31] = 'R';
    starPos[32] = 'L';
    starPos[33] = 'L';
    starPos[34] = 'L';
    starPos[35] = 'L';
    starPos[36] = 'R';
    starPos[37] = 'R';
    starPos[38] = 'L';
    starPos[39] = 'L';
    starPos[40] = 'L';
    starPos[41] = 'L';
    starPos[42] = 'R';
    starPos[43] = 'R';

    // Row 1
    starLine[0] = 1;
    starLine[1] = 2;
    starLine[2] = 3;
    starLine[3] = 4;
    starLine[4] = 5;
    starLine[5] = 6;
    starLine[6] = 7;

    //Row 2
    starLine[7] = 1;
    starLine[8] = 2;
    starLine[9] = 3;
    starLine[10] = 4;
    starLine[11] = 5;
    starLine[12] = 6;

    // Row 3
    starLine[13] = 1;
    starLine[14] = 2;
    starLine[15] = 3;
    starLine[16] = 4;
    starLine[17] = 5;
    starLine[18] = 6;

   // Row 4
    starLine[19] = 1;
    starLine[20] = 2;
    starLine[21] = 3;
    starLine[22] = 4;
    starLine[23] = 5;
    starLine[24] = 6;

   // Row 5
    starLine[25] = 0;
    starLine[26] = 1;
    starLine[27] = 2;
    starLine[28] = 3;
    starLine[29] = 4;
    starLine[30] = 5;
    starLine[31] = 6;

   // Row 6
    starLine[32] = 0;
    starLine[33] = 1;
    starLine[34] = 2;
    starLine[35] = 3;
    starLine[36] = 4;
    starLine[37] = 5;

   // Row 7
    starLine[38] = 0;
    starLine[39] = 1;
    starLine[40] = 2;
    starLine[41] = 3;
    starLine[42] = 4;
    starLine[43] = 5;


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

void StarField::fadeLine(int line, int r,int g,int b, double duration) {
    for (int i = 0; i < STAR_COUNT; i++) {
        if (starLine[i] == line) {
            stars[i]->fadeTo(r,g,b,duration);
        }
    }
}

void StarField::setLine(int line, int r,int g,int b) {
    for (int i = 0; i < STAR_COUNT; i++) {
        if (starLine[i] == line) {
            stars[i]->set(r,g,b);
        }
    }
}


void StarField::fadeLeft(int r, int g, int b, double duration)
{
    for (int i =0; i < STAR_COUNT; i++)
    {
        if ('L' == starPos[i])
            stars[i]->fadeTo(r,g,b,duration);
    }
}

void StarField::setLeft(int r, int g, int b)
{
    for (int i =0; i < STAR_COUNT; i++)
    {
        if ('L' == starPos[i])
            stars[i]->set(r,g,b);
    }
}


void StarField::fadeRight(int r, int g, int b, double duration)
{
    for (int i =0; i < STAR_COUNT; i++)
    {
        if ('R' == starPos[i])
            stars[i]->fadeTo(r,g,b,duration);
    }
}

void StarField::setRight(int r, int g, int b)
{
    for (int i =0; i < STAR_COUNT; i++)
    {
        if ('R' == starPos[i])
            stars[i]->set(r,g,b);
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

void StarField::setAll(int r, int g, int b) {
    for(int i =0; i < STAR_COUNT; i++) {
        stars[i]->set(r,g,b);
    }
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
