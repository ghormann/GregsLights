#ifndef E131NETWORK_H
#define E131NETWORK_H

#include "LightNetwork.h"
#include <netinet/in.h>


#define E131_PACKET_LEN 638
#define E131_PORT 5568
#define XLIGHTS_UUID "c0de0080-c69b-11e0-9572-0800200c9a66"


class E131Network : public LightNetwork
{
    public:
        E131Network(char *ipAddr);
        virtual ~E131Network();
        void doUpdate();
    protected:
    private:
        struct sockaddr_in myaddr, remoteaddr;
        int fd;
};

#endif // E131NETWORK_H
