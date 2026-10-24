#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#define MESSAGE_GENERATOR_SIZE 21

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
    std::string getMilestoneAnnouncement(int milestone);
    std::string getMilestoneTime(int milestone);
    static std::string formatNumber(int n);

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
