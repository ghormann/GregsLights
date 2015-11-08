#ifndef LORNETWORK_H
#define LORNETWORK_H

#include "FadeableBulb.h"
#include "time.h"
#include "LightNetwork.h"
#include "serial.h"


#define convertIntensity(i) ( (i) <= 0 ? 240 : ( (i) >= 100 ? 1 : 228 - 2*(i) ) )


class LORNetwork : public LightNetwork
{
public:
    LORNetwork(char * deviceName, bool sendData);
    Bulb* getBulb(int device, int channel);
    bool doUpdate(bool force);
    void setShutdown(bool val);
    virtual ~LORNetwork();
    SerialPort* serptr;
protected:
private:
    struct timespec last_ts;
};


class LORBulb : public FadeableBulb
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
