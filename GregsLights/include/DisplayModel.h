#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include "controller/RGBLight.h"
#include "controller/Bush.h"
#include "controller/NetworkCollection.h"
#include "CountdownClock.h"


#define HOUSE_LIGHT_START 1
#define HOUSE_LIGHT_END   4
#define BUSH_LIGHT_START 1
#define BUSH_LIGHT_END   6

#define MAX_MESSAGE_LENGTH 70
#define NUM_MESSAGE_BUFFERS 10

class DisplayModel
{
    public:
        DisplayModel(bool sendDMX);
        virtual ~DisplayModel();
        RGBLight *getHouse(int id);
        Bush *getBush(int id);
        CountdownClock *getClock();
        void setMessage(int id, char*);
        char *getMessage(int id);
    protected:
    private:
        NetworkCollection *networks;
        RGBLight *house[HOUSE_LIGHT_END + 1];
        Bush *bushes[BUSH_LIGHT_END+1];
        CountdownClock *clock;
        char *messages[NUM_MESSAGE_BUFFERS];

};

#endif // DISPLAYMODEL_H
