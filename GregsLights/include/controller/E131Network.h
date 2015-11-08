#ifndef E131NETWORK_H
#define E131NETWORK_H

#include "LightNetwork.h"
#include "DMXBulb.h"
#include "RGBLight.h"
#include <netinet/in.h>


#define E131_PACKET_LEN 638
#define E131_PORT 5568
//#define XLIGHTS_UUID "c0de0080-c69b-11e0-9572-0800200c9a66"
#define XLIGHTS_UUID "c0de0080c69b11e095720800200c9a66"


class E131Network : public LightNetwork
{
    public:
        E131Network(char *ipAddr, int universeNumber, int numChannels);
        virtual ~E131Network();
        void setIntensity(int id, unsigned char pct);
        void doUpdate();
        void setShutdown(bool val);
        void setDebug(bool val);
        RGBLight* getRGB(int start);
        Bulb* getBulb(int channel);
    protected:
    private:
        struct sockaddr_in myaddr, remoteaddr;
        static int fd;
        unsigned char data[E131_PACKET_LEN];
        unsigned char sequenceNum;
        int skipCount;
        bool xNetwork_E131_changed;
        bool debug;
        bool isShutdown;
        int num_channels;
        int universe;
        char *ipAsChar;
};

class E131Bulb : public FadeableBulb
{
public:
    E131Bulb(unsigned char *data, bool *flag);
    void setIntensity(int pct);
protected:
    void setIntensity_ipml(int pct);
    int getMin()
    {
        return 0;
    }
    int getMax()
    {
        return 255;
    }
private:
    unsigned char* pos;
    bool *flag;
};


#endif // E131NETWORK_H
