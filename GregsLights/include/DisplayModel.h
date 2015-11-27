#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include "controller/RGBLight.h"
#include "controller/Bush.h"
#include "controller/NetworkCollection.h"
#include "CountdownClock.h"
#include "Sign.h"
#include "Snowmen.h"
#include "LargeGrid.h"
#include "StarField.h"


#define HOUSE_LIGHT_START 1
#define HOUSE_LIGHT_END   4
#define BUSH_LIGHT_START 1
#define BUSH_LIGHT_END   6

#define MAX_MESSAGE_LENGTH 70
#define NUM_MESSAGE_BUFFERS 10

class DisplayModel
{
    public:
        DisplayModel(bool sendDMX, int skip_time_check, int show_new_year);
        virtual ~DisplayModel();
        RGBLight *getHouse(int id);
        Bush *getBush(int id);
        CountdownClock *getClock();
        Sign * getSign();
        Snowmen * getSnowmen();
        LargeGrid *getGrid();
        StarField *getStars();
        void setMessage(int id, char*);
        char *getMessage(int id);
        bool isSkipTimeCheck();
        bool isNewYears();
        void shutdown();
    protected:
    private:
        NetworkCollection *networks;
        NetworkCollection *networkAlpha1;
        NetworkCollection *networkAlpha2;
        NetworkCollection *networkClock;
        RGBLight *house[HOUSE_LIGHT_END + 1];
        Bush *bushes[BUSH_LIGHT_END+1];
        CountdownClock *clock;
        char *messages[NUM_MESSAGE_BUFFERS];
        Sign *sign;
        Snowmen *snowmen;
        LargeGrid *grid;
        StarField *stars;
        bool skipTimeCheck;
        bool newYears;

};

#endif // DISPLAYMODEL_H
