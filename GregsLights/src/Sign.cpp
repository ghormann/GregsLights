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
    while (true)
    {
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            this->pixals[i]->set(100,0,0);
            usleep(100000);
        }
        sleep(5);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++) {
            this->pixals[i]->set(0,100,0);
            usleep(100000);
        }
        sleep(5);
    }
}
