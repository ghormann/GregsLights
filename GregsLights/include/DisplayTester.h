#ifndef DISPLAYTESTER_H
#define DISPLAYTESTER_H

#include "DisplayModel.h"


class DisplayTester
{
    public:
        DisplayTester(DisplayModel *model);
        virtual ~DisplayTester();
        void testDMX();
        void testClock();
        void testSign();
        void testAll();
        static void * clockThread(void *);
        static void * dmxThread(void *);
        static void * signThread(void *);
    protected:
    private:
        DisplayModel *model;
};

#endif // DISPLAYTESTER_H
