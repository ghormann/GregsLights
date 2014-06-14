#include <iostream>
#include "../include/DummyPixal.h"

/*
 * THis is a dumy implement of Bulb that just prints changes to the screen
 */

DummyPixal::DummyPixal(int newid)
{
    this->id = newid;
    this->intensity = 0;
}

int DummyPixal::Getid()
{
    return id;
}

int DummyPixal::getMax()
{
    return 255;
}

int DummyPixal::getMin()
{
    return 0;
}


void DummyPixal::setIntensity_ipml(int i)
{
    using namespace std;
    this->intensity = i;
    cout << "Set Intensity for " << id << " to " << i << endl;
}

DummyPixal::~DummyPixal()
{
    //dtor
}
