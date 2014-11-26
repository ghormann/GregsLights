#include "../include/Sign.h"
#include <unistd.h>

Sign::Sign(E131Network *n1)
{
    this->net1 = n1;
    for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
    {
        this->pixals[i] = n1->getRGB(i*3);
    }
}

Sign::~Sign()
{
    //dtor
}

void Sign::test()
{
    int betweenPixals = 2000;
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
