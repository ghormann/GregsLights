/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "include/Bush.h"
#include "include/IPixal.h"
#include "include/DummyPixal.h"
#include "include/RGBLight.h"
#include "include/OpenDMXNetwork.h"
#include "include/LORNetwork.h"

using namespace std;

OpenDMXNetwork *dmx;
LORNetwork *lor;
void *serial_main(void *args);


int main()
{
    int i = 0;
    pthread_t serial_t;  /* Thread for writing to serial interface */
    dmx = new OpenDMXNetwork((char *)"/dev/ttyUSB0");
    lor = new LORNetwork((char*) "/dev/usb005");
    RGBLight *light1 = dmx->getRGB(7);
    RGBLight *light2 = dmx->getRGB(10);
    RGBLight *light3 = dmx->getRGB(50);
    light1->set(60,50,100);
    light2->set(25,50,75);
    light3->set(100,100,100);

    pthread_create(&serial_t, NULL, serial_main, (void*) NULL);
    sleep(1);
    for (i = 0; i < 100; i++)
    {
        printf("Loop: %d", i);
        light1->set(i,i,i);
        light2->set(i,i,i);
        sleep(1);
    }

    sleep(5);
    return 0;
}
void * serial_main(void *args)
{
    while (1)
    {
        dmx->doUpdate();
        lor->doUpdate();
        usleep(50 * 1000); // 50ms
    }

    return NULL;
}
