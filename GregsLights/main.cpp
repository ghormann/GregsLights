/*
 * Drive code for the the application
 * Author: Greg Hormann - ghormann@gmail.com
 */

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "include/DisplayModel.h"
#include "include/DisplayTester.h"
#include "include/TextDisplay.h"
#include "include/GregsDisplay.h"

using namespace std;
DisplayModel *model = 0;

void my_handler(int s)
{
    printf("Caught signal %d\n",s);
    if (model != 0)
    {
        model->shutdown();
    }
    exit(1);

}


int main(int argc, char *argv[])
{
    int skip_time_check = FALSE;
    int show_new_year = FALSE;
    int isDebug = FALSE;
    int ch;

    //Setup SIgnal Handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);


    /*
    * Parse Arguments
    */
    while ((ch = getopt(argc, argv, "dDtTyY")) != -1)
    {
        switch (ch)
        {
        case 'y':
        case 'Y':
            show_new_year = TRUE;
            break;
        case 'd':
        case 'D':
            isDebug = TRUE;
            break;
        case 't':
        case 'T':
            skip_time_check = TRUE;
            break;
        case '?':
        default:
            printf("Usage: %s [-d] [-t] [-y]\n", argv[0]);
            exit(1);
            break;
        }
    }
    argc -= optind;
    argv += optind;


    try
    {
        bool sendDMX = false;
        model = new DisplayModel(sendDMX, skip_time_check, show_new_year );
        sleep(1); // Allow threads to start up
        new TextDisplay(model);

        if (isDebug == TRUE)
        {
            DisplayTester *tester = new DisplayTester(model);
            tester->testAll();
        }
        else
        {
            GregsDisplay *display = new GregsDisplay(model);
            display->run();
        }

        while(1)
        {
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
