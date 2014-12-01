/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include "include/DisplayModel.h"
#include "include/DisplayTester.h"
#include "include/TextDisplay.h"
#include "include/GregsDisplay.h"

using namespace std;


int main()
{
    try
    {
        bool sendDMX = false;
        DisplayModel *model = new DisplayModel(sendDMX);
        sleep(1); // Allow threads to start up
        new TextDisplay(model);
        //DisplayTester *tester = new DisplayTester(model);
        //tester->testAll();

        GregsDisplay *display = new GregsDisplay(model);
        display->run();

        while(1) {
            sleep(60);
        }

    }
    catch (const char* msg)
    {
        cerr << msg << endl;
        return 1;
    }

    return 0;
}
