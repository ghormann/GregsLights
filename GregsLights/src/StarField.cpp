#include "../include/StarField.h"
#include "../include/controller/FadeableBulb.h"
#include <iostream>

using namespace std;

StarField::StarField(E131Network *net)
{
    //ctor
    // Start at DMX 100, but really 99 in E131
    for (int i = 0; i < STAR_COUNT; ++i)
    {
        stars[i] = net->getRGB(99 + i*3);
    }
}

StarField::~StarField()
{
    //dtor
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

void StarField::test()
{
    //((FadeableBulb *)stars[9]->getRedBulb())->setDebug(true);
    double duration = 5.0;
    RGBColor *colors[3];
    colors[0] = RGBColor::RED;
    colors[1] = RGBColor::GREEN;
    colors[2] = RGBColor::BLUE;

    while(1)
    {
        stars[0]->set(RGBColor::RED);
        sleep(1);
        stars[0]->set(RGBColor::GREEN);
        sleep(1);
        stars[0]->set(RGBColor::BLUE);
        sleep(1);
        stars[1]->set(RGBColor::GREEN);
        sleep(1);
        stars[9]->set(RGBColor::BLACK);
        stars[10]->set(RGBColor::BLACK);
        for (int i =0; i<2; i++)
        {
            for (int j=0; j < STAR_COUNT; j++)
            {
                int c = (i+j)%3;
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
