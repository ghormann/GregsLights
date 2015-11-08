#ifndef NETWORKCOLLECTION_H
#define NETWORKCOLLECTION_H

#define MAX_LIGHT_NETWORKS 50
#include "LightNetwork.h"
#include <pthread.h>
#include <unistd.h>


/**
 * Tracks the different networks that have been added for the purpose
 * of preforming updates.
*/

class NetworkCollection
{
    public:
        NetworkCollection(char *name);
        virtual ~NetworkCollection();
        void addNetwork( LightNetwork *net);
        void removeNetwork ( LightNetwork *net);
        void doUpdate();
        void doShutdown();
        void setControllerLimits(int maxUpdates, int sleepMS);
    protected:
    private:
        LightNetwork* networks[MAX_LIGHT_NETWORKS];
        int maxBeforeSleep;
        int sleepDuration;
        char name[80];
        pthread_t serial_t;  /* Thread for writing to serial interface */
        pthread_t bulb_t;     /* Thread for refershing bulbs */

};

#endif // NETWORKCOLLECTION_H
