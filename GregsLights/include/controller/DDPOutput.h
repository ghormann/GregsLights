#ifndef DDPOUTPUT_H_INCLUDED
#define DDPOUTPUT_H_INCLUDED

#define DDP_PORT 4048
#define DDP_HEADER_LEN 10

#define DDP_FLAGS1_VER     0xc0   // version mask
#define DDP_FLAGS1_VER1    0x40   // version=1
#define DDP_FLAGS1_PUSH    0x01
#define DDP_FLAGS1_QUERY   0x02
#define DDP_FLAGS1_REPLY   0x04
#define DDP_FLAGS1_STORAGE 0x08
#define DDP_FLAGS1_TIME    0x10

#define DDP_ID_DISPLAY       1
#define DDP_ID_CONFIG      250
#define DDP_ID_STATUS      251

//1440 channels per packet
#define DDP_CHANNELS_PER_PACKET 1440

#define DDP_PACKET_LEN (DDP_HEADER_LEN + DDP_CHANNELS_PER_PACKET)


#include <string>
#include <netinet/in.h>
#include "LightNetwork.h"
#include "DMXBulb.h"
#include "RGBLight.h"


class DDPOutput : public LightNetwork
{
private:
    std::string ip_address;
    int maxPixels;
    int maxPackets;
    bool isShutdown;
    unsigned char *data;
    struct sockaddr_in myaddr, remoteaddr;
    int fd;
    int lastPacketLength;
    void setPixel(int id, unsigned char r, unsigned char g, unsigned char b);
    unsigned char * getPosition(int channelId);

public:
    DDPOutput(std::string ip, int numPixels, int startChannel);
    bool doUpdate(bool force);
    void setShutdown(bool val);
    RGBLight* getRGB(int start);
    Bulb* getBulb(int channel);

    virtual ~DDPOutput();
};

class DDPBulb : public FadeableBulb
{
public:
    DDPBulb(unsigned char *data);
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
};




#endif // DDPOUTPUT_H_INCLUDED
