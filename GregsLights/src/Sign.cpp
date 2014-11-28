#include "../include/Sign.h"
#include <unistd.h>
#include "../include/controller/DummyBulb.h"

Sign::Sign(E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4, E131Network *n5, E131Network *n6)
{
    int cnt = 0;
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n1->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[cnt++] = n2->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[cnt++] = n3->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[cnt++] = n4->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++) {
        this->pixals[cnt++] = n5->getRGB(i*3);
    }
    for (int i = 0; i < 110; i++) {
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
    if (i < 0 || i >= TOTAL_SIGN_PIXALS) {
        throw "Sign::getPixal must be between 0 and TOTAL_SIGN_PIXALS";
    }
    return this->pixals[i];
}

RGBLight *Sign::getPixal(int x, int y)
{
    return getPixal(y*SIGN_WIDTH + x);
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
