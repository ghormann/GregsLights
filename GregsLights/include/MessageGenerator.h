#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#define MESSAGE_GENERATOR_SIZE 9

#include "TimeInfo.h"

class MessageGenerator
{
    public:
        MessageGenerator();
        virtual ~MessageGenerator();
        char * getMessage();
    protected:
    private:
        bool isChristmas;
        void clear();
        void checkClear();
        char bitmap[MESSAGE_GENERATOR_SIZE];
        TimeInfo *timeInfo;
        char message[80];
};

#endif // MESSAGEGENERATOR_H