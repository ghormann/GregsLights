#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#define MESSAGE_GENERATOR_SIZE 16

#include "TimeInfo.h"
#include <string>

class MessageGenerator
{
public:
    MessageGenerator(TimeInfo *ti);
    virtual ~MessageGenerator();
    std::string getMessage();
    const char * getGreeting();
    std::string getStartDate();

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
