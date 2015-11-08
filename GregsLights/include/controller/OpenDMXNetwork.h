#ifndef OPENDMXNETWORK_H
#define OPENDMXNETWORK_H

#include "Bulb.h"
#include "RGBLight.h"
#include "LightNetwork.h"
#include "serial.h"
#include "DMXBulb.h"

/*
 * This version works with the programer from Holiday Coro
 */

 enum RGB_TYPE{
  OPENDMX = 0,
  ACTIDONGLE
};


class OpenDMXNetwork : public LightNetwork
{
public:
    OpenDMXNetwork(char * deviceName, RGB_TYPE type, bool sendData);
    RGBLight* getRGB(int start);
    Bulb* getBulb(int channel);
    bool doUpdate(bool force);
    void setShutdown(bool val);
    virtual ~OpenDMXNetwork();
protected:
private:
    unsigned char data[513 + 6];
    SerialPort* serptr;
    int offset;
};



#endif // OPENDMXNETWORK_H
