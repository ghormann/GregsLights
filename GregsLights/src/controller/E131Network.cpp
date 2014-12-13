#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include "../../include/controller/E131Network.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


E131Network::E131Network(char *ipAddr, int universeNumber, int numChannels)
{

    if (universeNumber == 0 || universeNumber >= 64000)
    {
        throw "universe number must be between 1 and 63999";
    }

    if (numChannels > 512)
    {
        throw "max channels on E1.31 is 512";
    }


    memset(data,0,sizeof(data));
    sequenceNum=0;
    skipCount=0;
    num_channels=numChannels;
    xNetwork_E131_changed = false;
    unsigned char univHi = universeNumber >> 8;   // Universe Number (high)
    unsigned char univLo = universeNumber & 0xff; // Universe Number (low)


    //Setup My Address
    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family=AF_INET;
    myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    myaddr.sin_port=htons(0);

    //Setup reomote Address
    memset((char *) &remoteaddr, 0, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(E131_PORT);
    if (inet_aton(ipAddr, &remoteaddr.sin_addr)==0)
    {
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

    data[0]=0x00;   // RLP preamble size (high)
    data[1]=0x10;   // RLP preamble size (low)
    data[2]=0x00;   // RLP postamble size (high)
    data[3]=0x00;   // RLP postamble size (low)
    data[4]=0x41;   // ACN Packet Identifier (12 bytes)
    data[5]=0x53;
    data[6]=0x43;
    data[7]=0x2d;
    data[8]=0x45;
    data[9]=0x31;
    data[10]=0x2e;
    data[11]=0x31;
    data[12]=0x37;
    data[13]=0x00;
    data[14]=0x00;
    data[15]=0x00;
    data[16]=0x72;  // RLP Protocol flags and length (high)
    data[17]=0x6e;  // 0x26e = 638 - 16
    data[18]=0x00;  // RLP Vector (Identifies RLP Data as 1.31 Protocol PDU)
    data[19]=0x00;
    data[20]=0x00;
    data[21]=0x04;

    // CID/UUID

    unsigned char  msb,lsb;
    unsigned char id[] = XLIGHTS_UUID;
    for (int i=0,j=22; i < 32; i+=2)
    {
        msb=id[i];
        lsb=id[i+1];
        msb -= isdigit(msb) ? 0x30 : 0x57;
        lsb -= isdigit(lsb) ? 0x30 : 0x57;
        data[j++] = ((msb << 4) | lsb);
    }

    data[38]=0x72;  // Framing Protocol flags and length (high)
    data[39]=0x58;  // 0x258 = 638 - 38
    data[40]=0x00;  // Framing Vector (indicates that the E1.31 framing layer is wrapping a DMP PDU)
    data[41]=0x00;
    data[42]=0x00;
    data[43]=0x02;
    data[44]='x';   // Source Name (64 bytes)
    data[45]='L';
    data[46]='i';
    data[47]='g';
    data[48]='h';
    data[49]='t';
    data[50]='s';
    data[51]=0x00;
    data[52]=0x00;
    data[53]=0x00;
    data[54]=0x00;
    data[55]=0x00;
    data[56]=0x00;
    data[57]=0x00;
    data[58]=0x00;
    data[59]=0x00;
    data[60]=0x00;
    data[61]=0x00;
    data[61]=0x00;
    data[62]=0x00;
    data[63]=0x00;
    data[64]=0x00;
    data[65]=0x00;
    data[66]=0x00;
    data[67]=0x00;
    data[68]=0x00;
    data[69]=0x00;
    data[70]=0x00;
    data[71]=0x00;
    data[71]=0x00;
    data[72]=0x00;
    data[73]=0x00;
    data[74]=0x00;
    data[75]=0x00;
    data[76]=0x00;
    data[77]=0x00;
    data[78]=0x00;
    data[79]=0x00;
    data[80]=0x00;
    data[81]=0x00;
    data[81]=0x00;
    data[82]=0x00;
    data[83]=0x00;
    data[84]=0x00;
    data[85]=0x00;
    data[86]=0x00;
    data[87]=0x00;
    data[88]=0x00;
    data[89]=0x00;
    data[90]=0x00;
    data[91]=0x00;
    data[91]=0x00;
    data[92]=0x00;
    data[93]=0x00;
    data[94]=0x00;
    data[95]=0x00;
    data[96]=0x00;
    data[97]=0x00;
    data[98]=0x00;
    data[99]=0x00;
    data[100]=0x00;
    data[101]=0x00;
    data[101]=0x00;
    data[102]=0x00;
    data[103]=0x00;
    data[104]=0x00;
    data[105]=0x00;
    data[106]=0x00;
    data[107]=0x00;
    data[108]=100;  // Priority
    data[109]=0x00; // Reserved
    data[110]=0x00; // Reserved
    data[111]=0x00; // Sequence Number
    data[112]=0x00; // Framing Options Flags
    data[113]=univHi;  // Universe Number (high)
    data[114]=univLo;  // Universe Number (low)

    data[115]=0x72;  // DMP Protocol flags and length (high)
    data[116]=0x0b;  // 0x20b = 638 - 115
    data[117]=0x02;  // DMP Vector (Identifies DMP Set Property Message PDU)
    data[118]=0xa1;  // DMP Address Type & Data Type
    data[119]=0x00;  // First Property Address (high)
    data[120]=0x00;  // First Property Address (low)
    data[121]=0x00;  // Address Increment (high)
    data[122]=0x01;  // Address Increment (low)
    data[123]=0x02;  // Property value count (high)
    data[124]=0x01;  // Property value count (low)
    data[125]=0x00;  // DMX512-A START Code

    int i = num_channels + 1;
    unsigned char NumHi = i >> 8;   // Channels (high)
    unsigned char NumLo = i & 0xff; // Channels (low)

    data[123]=NumHi;  // Property value count (high)
    data[124]=NumLo;  // Property value count (low)

    i = 638 - 16 - (512 - num_channels);
    unsigned char hi = i >> 8;   // (high)
    unsigned char lo = i & 0xff; // (low)

    data[16]=hi + 0x70;  // RLP Protocol flags and length (high)
    data[17]=lo;  // 0x26e = 638 - 16

    i = 638 - 38 - (512 - num_channels);
    hi = i >> 8;   // (high)
    lo = i & 0xff; // (low)
    data[38]=hi + 0x70;  // Framing Protocol flags and length (high)
    data[39]=lo;  // 0x258 = 638 - 38

    i = 638 - 115 - (512 - num_channels);
    hi = i >> 8;   // (high)
    lo = i & 0xff; // (low)
    data[115]=hi + 0x70;  // DMP Protocol flags and length (high)
    data[116]=lo;  // 0x20b = 638 - 115
}

E131Network::~E131Network()
{

}


void E131Network::setIntensity(int id, unsigned char pct) {
    data[id+126] = pct;
    xNetwork_E131_changed = true;
}

void E131Network::doUpdate()
{
    //dtor
    if (xNetwork_E131_changed || skipCount > 50)  // SKip Level is very dependant on Time bewteen updates.  Currently 50ms
    {
        data[111]=sequenceNum;
        int slen=sizeof(remoteaddr);
        //datagram->SendTo(remoteAddr, data, E131_PACKET_LEN - (512 - num_channels));
         if (sendto(fd, data, E131_PACKET_LEN - (512 - num_channels), 0, (struct sockaddr *)&remoteaddr, slen)==-1) {
            throw "Error during sentdto in E1.31";
		}
        sequenceNum= sequenceNum==255 ? 0 : sequenceNum+1;
        skipCount=0;
        xNetwork_E131_changed = false;
    }
    else
    {
        skipCount++;
    }
}

RGBLight* E131Network::getRGB(int start)
{
    Bulb *r = this->getBulb(start);
    Bulb *g = this->getBulb(start+1);
    Bulb *b = this->getBulb(start+2);
    return new RGBLight(r,g,b);
}


Bulb* E131Network::getBulb(int channel)
{
    if (channel < 0 || channel > 512)
        throw "Invalid Channel";
    E131Bulb *rc = new E131Bulb(data+126+channel, &(this->xNetwork_E131_changed));
    return  rc;
}


E131Bulb::E131Bulb(unsigned char *data, bool *flagPtr) : Bulb(true)
{
    this->pos = data;
    this->flag = flagPtr;
}

void E131Bulb::setIntensity_ipml(int val)
{
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    pos[0] = (char) val;
    *(this->flag) = true;  // Mark Dirty
}
