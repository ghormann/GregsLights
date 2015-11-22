#include "../include/StarField.h"
#include "../include/controller/FadeableBulb.h"

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

void StarField::test()
{
    ((FadeableBulb *)stars[0]->getRedBulb())->setDebug(true);
    double duration = 5.0;
    RGBColor *colors[3];
    colors[0] = RGBColor::RED;
    colors[1] = RGBColor::GREEN;
    colors[2] = RGBColor::BLUE;

    while(1)
    {
        for (int i =0; i<2; i++)
        {
            for (int j=0; j < STAR_COUNT; j++)
            {
                int c = (i+j)%3;
                stars[j]->fadeTo(colors[c], duration);
            }

            sleep(duration);
        }

        for (int j = 0; j < STAR_COUNT; j++) {
            stars[j]->fadeTo(RGBColor::BLACK, duration);
        }
        sleep(duration);
    }
}
