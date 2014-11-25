#include "../include/Sign.h"
#include <unistd.h>

Sign::Sign(E131Network *n1)
{
    this->net1 = n1;
}

Sign::~Sign()
{
    //dtor
}

void Sign::test()
{
    while (true)
    {
        for (int i =0; i < 150; i++)
        {
            net1->setIntensity(i,100);
        }
        sleep(5);

        for (int i = 0; i < 50; i++)
        {
            net1->setIntensity(i,100);
            usleep(300000);
            net1->setIntensity(i,0);
        }
    }

}
