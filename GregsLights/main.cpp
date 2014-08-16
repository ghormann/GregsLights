/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include "include/controller/DisplayModel.h"
#include "include/controller/DisplayTester.h"

using namespace std;


int main()
{
    bool sendDMX = false;
    DisplayModel *model = new DisplayModel(sendDMX);
    DisplayTester *tester = new DisplayTester(model);

    tester->testDMX();
    //testLOR(lor);

    return 0;
}
