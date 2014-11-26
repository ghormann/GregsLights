#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"

#define TOTAL_SIGN_PIXALS 50

class Sign
{
    public:
        Sign(E131Network *n1);
        virtual ~Sign();
        void test();
    protected:
    private:
        E131Network *net1;
        RGBLight *pixals[TOTAL_SIGN_PIXALS];
};

#endif // SIGN_H
