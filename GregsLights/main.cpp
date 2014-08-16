/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include "include/DisplayModel.h"
#include "include/DisplayTester.h"

using namespace std;


int main()
{
    DisplayModel *model = new DisplayModel(true);
    DisplayTester *tester = new DisplayTester(model);

    tester->testDMX();
    //testLOR(lor);

    return 0;
}
