#ifndef DUMMYPIXAL_H
#define DUMMYPIXAL_H

#include "IPixal.h"


class DummyPixal : public IPixal
{
    public:
        DummyPixal(int id);
        virtual ~DummyPixal();
        int Getid();
        int getMin();
        int getMax();
    protected:
        virtual void setIntensity_ipml(int value);
    private:
        int id;
        int intensity;
};

#endif // DUMMYPIXAL_H
