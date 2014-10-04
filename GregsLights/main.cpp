/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include "include/DisplayModel.h"
#include "include/DisplayTester.h"
#include "include/TextDisplay.h"

using namespace std;


int main()
{
    try
    {
        bool sendDMX = false;
        DisplayModel *model = new DisplayModel(sendDMX);
        DisplayTester *tester = new DisplayTester(model);

        sleep(1); // Allow threads to start up
        new TextDisplay(model);

        tester->testDMX();
        //testLOR(lor);

    }
    catch (const char* msg)
    {
        cerr << msg << endl;
        return 1;
    }

    return 0;
}
