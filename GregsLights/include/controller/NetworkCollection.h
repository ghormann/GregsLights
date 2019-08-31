#ifndef NETWORKCOLLECTION_H
#define NETWORKCOLLECTION_H

#define MAX_LIGHT_NETWORKS 100
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
        NetworkCollection(char *name, int msBetween, int maxTicks,int maxBeforeSleep, int extraSleepMs, bool changeOrder);
        virtual ~NetworkCollection();
        void addNetwork( LightNetwork *net);
        void removeNetwork ( LightNetwork *net);
        void doUpdate();
        void doShutdown();
        void setClientLock(pthread_mutex_t* lock);
        void setControllerLimits(int maxUpdates, int sleepMS);
        int msBetween;
    protected:
    private:
        pthread_mutex_t* clientLock = NULL;
        LightNetwork* networks[MAX_LIGHT_NETWORKS];
        int counts[MAX_LIGHT_NETWORKS];  // Ticks since last update
/*
 * msBetween: Millisonds between checking for updates (Tick)
 * maxTicks: Max number of updates skipped before force
 * maxBeforeSleep: Max Universes updated before taking an extra sleep
 * extraSleepMs: Duration (MS) of sleep when MaxBeforeSleep hit.
 * if changeOrder, then send the universers in differ starting piont each time.
 */
        int maxTicks;
        int maxBeforeSleep;
        int extraSleepMs;
        int startAt;
        bool changeOrder;
        char name[80];
        pthread_t serial_t;  /* Thread for writing to serial interface */
        pthread_t bulb_t;     /* Thread for refershing bulbs */

};

#endif // NETWORKCOLLECTION_H
