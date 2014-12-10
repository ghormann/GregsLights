#ifndef MESSAGEGENERATOR_H
#define MESSAGEGENERATOR_H


class MessageGenerator
{
    public:
        MessageGenerator();
        virtual ~MessageGenerator();
        char * getMessage();
    protected:
    private:
        bool isChristmas;
        int lastId;
};

#endif // MESSAGEGENERATOR_H
