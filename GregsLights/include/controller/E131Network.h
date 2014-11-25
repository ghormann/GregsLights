#ifndef E131NETWORK_H
#define E131NETWORK_H

#include "LightNetwork.h"
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
        void test();
    protected:
    private:
        struct sockaddr_in myaddr, remoteaddr;
        int fd;
        unsigned char data[E131_PACKET_LEN];
        unsigned char sequenceNum;
        int skipCount;
        bool xNetwork_E131_changed;
        int num_channels;
};

#endif // E131NETWORK_H
