#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include "controller/RGBLight.h"
#include "controller/Bush.h"
#include "controller/NetworkCollection.h"
#include "CountdownClock.h"
#include "Sign.h"
#include "GarageSign.h"
#include "Snowmen.h"
#include "GregMQTT.h"
#include "LargeGrid.h"

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
        CountdownClock *getClock();
        LargeGrid* getLargeGrid();
        Sign * getSign();
        Snowmen * getSnowmen();
        GarageSign *getGarageSign();
        void setMessage(int id, char*);
        char *getMessage(int id);
        bool isSkipTimeCheck();
        bool isNewYears();
        void shutdown();
        GregMQTT *getMqtt();
    protected:
    private:
        NetworkCollection *networks;
        NetworkCollection *networkClock;
        NetworkCollection *networkSnowman;
        NetworkCollection *networkP10;
        GregMQTT *mqtt;
        CountdownClock *clock;
        char *messages[NUM_MESSAGE_BUFFERS];
        Sign *sign;
        Snowmen *snowmen;
        GarageSign *garageSign;
        LargeGrid *largeGrid;
        bool skipTimeCheck;
        bool newYears;

};

#endif // DISPLAYMODEL_H
