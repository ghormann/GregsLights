#ifndef DISPLAYMODEL_H
#define DISPLAYMODEL_H

#include "controller/RGBLight.h"
#include "controller/NetworkCollection.h"
#include "CountdownClock.h"


#define HOUSE_LIGHT_START 1
#define HOUSE_LIGHT_END   4

class DisplayModel
{
    public:
        DisplayModel(bool sendDMX);
        virtual ~DisplayModel();
        RGBLight *getHouse(int id);
    protected:
    private:
        NetworkCollection *networks;
        RGBLight *house[HOUSE_LIGHT_END + 1];
        CountdownClock *clock;

};

#endif // DISPLAYMODEL_H
