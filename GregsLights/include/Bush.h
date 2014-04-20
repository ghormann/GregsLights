#ifndef BUSH_H
#define BUSH_H

#include "IPixal.h"

class Bush
{
public:
    Bush(int id,IPixal *red, IPixal *green, IPixal *blue, IPixal *white);
    void setAll(int pct);
    int getId();
    virtual ~Bush();
protected:
private:
    int bushId;
    IPixal *red;
    IPixal *green;
    IPixal *blue;
    IPixal *white;
};

#endif // BUSH_H
