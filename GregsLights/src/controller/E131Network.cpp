
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include "../../include/controller/E131Network.h"
#include <netinet/in.h>
#include <arpa/inet.h>

E131Network::E131Network(char *ipAddr)
{
        //Setup My Address
        memset(&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family=AF_INET;
        myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
        myaddr.sin_port=htons(0);

        //Setup reomote Address
        memset((char *) &remoteaddr, 0, sizeof(remoteaddr));
        remoteaddr.sin_family = AF_INET;
        remoteaddr.sin_port = htons(E131_PORT);
        if (inet_aton(ipAddr, &remoteaddr.sin_addr)==0) {
            throw  "inet_aton() failed for remote addr\n";
        }


        // Setup Socket
        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            throw "cannot create socket" ;
        }

        if(bind(fd,( struct sockaddr *) &myaddr, sizeof(myaddr))<0)
        {
            throw "Bind to Socked failed";
        }

//        char *buf = "This is junk";
//        int slen=sizeof(remoteaddr);
//
//        if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remoteaddr, slen)==-1) {
//			perror("sendto");
//			exit(1);
//		}

}

E131Network::~E131Network()
{
    //dtor
}

void E131Network::doUpdate() {

}
