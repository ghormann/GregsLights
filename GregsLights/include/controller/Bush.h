#ifndef BUSH_H
#define BUSH_H

#include "Bulb.h"

class Bush
{
public:
    Bush(int id,Bulb *red, Bulb *green, Bulb *blue, Bulb *white);
    void setAll(int pct);
    Bulb *getRed();
    Bulb *getGreen();
    Bulb *getBlue();
    Bulb *getWhite();
    int getId();
    virtual ~Bush();
protected:
private:
    int bushId;
    Bulb *red;
    Bulb *green;
    Bulb *blue;
    Bulb *white;
};

#endif // BUSH_H
