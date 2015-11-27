#include "../include/StarField.h"
#include "../include/controller/FadeableBulb.h"
#include <iostream>

using namespace std;

StarField::StarField(E131Network *net)
{
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
	
    
}

StarField::~StarField()
{
    //dtor
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

void StarField::test()
{
    //((FadeableBulb *)stars[9]->getRedBulb())->setDebug(true);
    double duration = 5.0;
    RGBColor *colors[3];
    colors[0] = RGBColor::RED;
    colors[1] = RGBColor::GREEN;
    colors[2] = RGBColor::BLUE;

    while(1) {
	int i ;
	for (i =38; i < STAR_COUNT; i++) {
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
    while(0)
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
