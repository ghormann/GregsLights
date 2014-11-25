#ifndef SIGN_H
#define SIGN_H

#include "controller/E131Network.h"

class Sign
{
    public:
        Sign(E131Network *n1);
        virtual ~Sign();
        void test();
    protected:
    private:
        E131Network *net1;
};

#endif // SIGN_H
