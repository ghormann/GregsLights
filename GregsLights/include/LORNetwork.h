#ifndef LORNETWORK_H
#define LORNETWORK_H

#include "IPixal.h"
#include "time.h"

#define convertIntensity(i) ( (i) <= 0 ? 240 : ( (i) >= 100 ? 1 : 228 - 2*(i) ) )


class LORNetwork
{
public:
    LORNetwork(char * deviceName);
    IPixal* getPixal(int device, int channel);
    virtual ~LORNetwork();
    void doUpdate();
protected:
private:
    struct timespec last_ts;
};


class LORPixal : public IPixal
{
public:
    LORPixal(unsigned char device, unsigned char channel, LORNetwork *network);
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
