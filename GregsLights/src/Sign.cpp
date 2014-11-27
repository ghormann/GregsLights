#include "../include/Sign.h"
#include <unistd.h>

Sign::Sign(E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4)
{
    for (int i = 0; i < 170; i++)
    {
        this->pixals[i] = n1->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[i+170] = n2->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[i+170+170] = n3->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[i+170+170+170] = n4->getRGB(i*3);
    }

}

Sign::~Sign()
{
    //dtor
}

RGBLight * Sign::getPixal(int i)
{
    if (i < 0 || i >= TOTAL_SIGN_PIXALS) {
        throw "Sign::getPixal must be between 0 and TOTAL_SIGN_PIXALS";
    }
    return this->pixals[i];
}

void Sign::test()
{
    int betweenPixals = 20000;
    int betweenColors = 2;
    while (true)
    {
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            this->pixals[i]->set(100,0,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            this->pixals[i]->set(0,100,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            this->pixals[i]->set(0,0,100);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            this->pixals[i]->set(100,100,100);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            pixals[i]->fade(100,100,100,100,0,0,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            pixals[i]->fade(100,0,0,0,100,0,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            pixals[i]->fade(0,100,0,0,0,100,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            pixals[i]->fade(0,0,100,0,0,0,betweenColors);
        }
        sleep(betweenColors);
        sleep(2);
    }
}
