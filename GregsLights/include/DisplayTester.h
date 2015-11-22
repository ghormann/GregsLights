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
        void testBushes();
        void testAll();
        static void * clockThread(void *);
        static void * dmxThread(void *);
        static void * bushThread(void *);
        static void * signThread(void *);
        static void * snowmenThread(void *);
        static void * gridThread(void *);
        static void * starThread(void *);
    protected:
    private:
        DisplayModel *model;
};

#endif // DISPLAYTESTER_H
