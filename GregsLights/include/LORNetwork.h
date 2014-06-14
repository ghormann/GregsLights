#ifndef LORNETWORK_H
#define LORNETWORK_H

#include "Bulb.h"
#include "time.h"
#include "LightNetwork.h"

#define convertIntensity(i) ( (i) <= 0 ? 240 : ( (i) >= 100 ? 1 : 228 - 2*(i) ) )


class LORNetwork : public LightNetwork
{
public:
    LORNetwork(char * deviceName);
    Bulb* getBulb(int device, int channel);
    void doUpdate();
    virtual ~LORNetwork();
protected:
private:
    struct timespec last_ts;
};


class LORBulb : public Bulb
{
public:
    LORBulb(unsigned char device, unsigned char channel, LORNetwork *network);
protected:
    void setIntensity_ipml(int pct);
    int getMin()
    {
        return 0;
    }
    int getMax()
    {
        return 100;
    }
private:
    unsigned char device;
    unsigned char channel;
    LORNetwork *network;
};



#endif // LORNETWORK_H
