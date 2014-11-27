#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"

#define TOTAL_SIGN_PIXALS 170*4

class Sign
{
    public:
        Sign(E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4);
        virtual ~Sign();
        void test();
        RGBLight * getPixal(int i);
    protected:
    private:
        E131Network *net1;
        RGBLight *pixals[TOTAL_SIGN_PIXALS];
};

#endif // SIGN_H
