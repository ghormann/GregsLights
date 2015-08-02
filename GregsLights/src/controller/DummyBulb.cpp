#include "DummyBulb.h"

DummyBulb::DummyBulb() : Bulb()
{
    //ctor
}

DummyBulb::~DummyBulb()
{
    //dtor
}

int DummyBulb::getMin()
{
    return 0;
}
int DummyBulb::getMax()
{
    return 100;
}

void DummyBulb::setIntensity_ipml(int pct)
{
    // Do nothing
}
