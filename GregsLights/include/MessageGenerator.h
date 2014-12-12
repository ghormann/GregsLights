#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H

#define MESSAGE_GENERATOR_SIZE 7

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
};

#endif // MESSAGEGENERATOR_H
