#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#define MESSAGE_GENERATOR_SIZE 12

#include "TimeInfo.h"

class MessageGenerator
{
    public:
        MessageGenerator(TimeInfo *ti);
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
