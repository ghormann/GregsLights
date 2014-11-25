#ifndef DISPLAYTESTER_H
#define DISPLAYTESTER_H

#include "DisplayModel.h"


class DisplayTester
{
    public:
        DisplayTester(DisplayModel *model);
        void testDMX();
        void testClock();
        void testSign();
        virtual ~DisplayTester();
    protected:
    private:
        DisplayModel *model;
};

#endif // DISPLAYTESTER_H
