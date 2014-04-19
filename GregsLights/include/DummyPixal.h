#ifndef DUMMYPIXAL_H
#define DUMMYPIXAL_H

/*
 * THis is a dumy implement of IPixal that just prints changes to the screen
 */

#include "IPixal.h"

class DummyPixal : public IPixal
{
public:
    DummyPixal(int id);
    virtual ~DummyPixal();
    int Getid();
protected:
    void setIntensity_ipml(int value);
    int getMin();
    int getMax();
private:
    int id;
    int intensity;
};

#endif // DUMMYPIXAL_H
