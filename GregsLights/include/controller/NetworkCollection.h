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
        NetworkCollection(char *name, int msBetween, int maxTicks,int maxBeforeSleep, int extraSleepMs);
        virtual ~NetworkCollection();
        void addNetwork( LightNetwork *net);
        void removeNetwork ( LightNetwork *net);
        void doUpdate();
        void doShutdown();
        void setControllerLimits(int maxUpdates, int sleepMS);
        int msBetween;
    protected:
    private:
        LightNetwork* networks[MAX_LIGHT_NETWORKS];
        int counts[MAX_LIGHT_NETWORKS];  // Ticks since last update
/*
 * msBetween: Millisonds between checking for updates (Tick)
 * maxTicks: Max number of updates skipped before force
 * maxBeforeSleep: Max Universes updated before taking an extra sleep
 * extraSleepMs: Duration (MS) of sleep when MaxBeforeSleep hit.
 */
        int maxTicks;
        int maxBeforeSleep;
        int extraSleepMs;
        char name[80];
        pthread_t serial_t;  /* Thread for writing to serial interface */
        pthread_t bulb_t;     /* Thread for refershing bulbs */

};

#endif // NETWORKCOLLECTION_H
