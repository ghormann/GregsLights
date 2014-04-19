/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include "include/Bush.h"
#include "include/IPixal.h"
#include "include/DummyPixal.h"
#include "include/RGBLight.h"
#include "include/OpenDMXNetwork.h"

using namespace std;

int main()
{
    cout << "Hello world!!!" << endl;
    OpenDMXNetwork *dmx = new OpenDMXNetwork("/dev/usb003");
    RGBLight *light1 = dmx->getRGB(1);
    RGBLight *light2 = dmx->getRGB(4);
    RGBLight *light3 = dmx->getRGB(7);
    light1->set(60,50,100);
    light2->set(25,50,75);
    light3->set(100,100,100);
    dmx->doUpdate(100);
    return 0;
}
