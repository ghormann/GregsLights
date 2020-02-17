#ifndef DISPLAYTESTER_H
#define DISPLAYTESTER_H

#include "DisplayModel.h"


class DisplayTester
{
    public:
        DisplayTester(DisplayModel *model);
        virtual ~DisplayTester();
        void testClock();
        void testSign();
        void testAll();
        void testGarage();
        static void * clockThread(void *);
        static void * signThread(void *);
        static void * snowmenThread(void *);
        static void * garageThread(void *);
    protected:
    private:
        DisplayModel *model;
};

#endif // DISPLAYTESTER_H
