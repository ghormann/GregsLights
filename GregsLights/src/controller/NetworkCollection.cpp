#include "../../include/controller/NetworkCollection.h"
#include "../../include/controller/FadeableBulb.h"
#include <stdio.h>
#include <string.h>

void *update_thread(void *args);


/*
 * msBetween: Millisonds between checking for updates (Tick)
 * maxTicks: Max number of updates skipped before force
 * maxBeforeSleep: Max Universes updated before taking an extra sleep
 * extraSleepMs: Duration (MS) of sleep when MaxBeforeSleep hit.
 */
NetworkCollection::NetworkCollection(char *name, int msBetween, int maxTicks,int maxBeforeSleep, int extraSleepMs, bool alwaysChangeOrder)
{
    strcpy(this->name, name);
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        networks[i] = 0;
        counts[i] = 0;
    }
    this->changeOrder = alwaysChangeOrder;
    this->startAt = 0;
    this->msBetween = msBetween;
    this->maxTicks = maxTicks;
    this->extraSleepMs = extraSleepMs;   // Duration of sleep if too may updates (in ms)
    this->maxBeforeSleep = maxBeforeSleep;   //Number of universers to update before extra sleep;

    pthread_create(&(this->serial_t), NULL, update_thread, (void*) this);
    pthread_create(&(this->bulb_t), NULL, FadeableBulb::tickThread, (void*) NULL);
}

void * update_thread(void *args)
{
    NetworkCollection *ptr = (NetworkCollection *)args;
    sleep(2);  // Allow other threads to start up before trying to send updates.
    while (1)
    {
        ptr->doUpdate();
        usleep(ptr->msBetween * 1000);
    }

    return NULL;
}

/*
 * ALpha Pix has problems if too many updates in short period of time.
 */
void NetworkCollection::setControllerLimits(int maxUpdates, int sleepMS)
{
    this->maxBeforeSleep = maxUpdates;
    this->extraSleepMs = sleepMS;
}


void NetworkCollection::doUpdate()
{

    /*
     * msBetween: Millisonds between checking for updates (Tick) (Used by Caller)
     * maxTicks: Max number of updates skipped before force
     * maxBeforeSleep: Max Universes updated before taking an extra sleep
     * extraSleepMs: Duration (MS) of sleep when MaxBeforeSleep hit.
     */


    int i = 0;
    int change = 0;
    bool force = false;
    //printf("Name: %s is starting at %d\n", name, startAt);
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        int netId = (i+startAt)%MAX_LIGHT_NETWORKS;
        if (networks[netId] != 0)
        {
            force = (counts[netId] > maxTicks);
            if (clientLock)
            {
                pthread_mutex_lock(clientLock);
            }
            bool didUpdate = networks[netId]->doUpdate(force);
            if (clientLock)
            {
                pthread_mutex_unlock(clientLock);
            }
            if (didUpdate)
            {
                ++change;
                counts[netId] = 0;
                if (++change > maxBeforeSleep)
                {
                    //printf("Sleeping longer for %s: %dms\n", this->name, extraSleepMs);
                    change = 0;
                    usleep(extraSleepMs*1000);
                }
            }
            else
            {
                ++(counts[netId]);
            }
            if (counts[netId] > 9999999)
            {
                counts[netId] = 0; // Prevent overflow
            }
        }
    }
    startAt = changeOrder ? (startAt+maxBeforeSleep-1)%16: 0;
}

void NetworkCollection::doShutdown()
{
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        if (networks[i]!= 0)
        {
            networks[i]->setShutdown(true);
        }
    }
}

void NetworkCollection::removeNetwork( LightNetwork *net)
{
    int i = 0;
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        if (networks[i] == net)
        {
            networks[i] = 0;
            break;
        }
    }
}

void NetworkCollection::setClientLock(pthread_mutex_t* lock)
{
    this->clientLock = lock;
}

void NetworkCollection::addNetwork( LightNetwork *net)
{
    int i = 0;
    int added = 0;
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        if (networks[i] == 0)
        {
            networks[i] = net;
            added = 1;
            break;
        }
    }
    if (! added)
    {
        throw "ERROR: NetworkCollection is Full";
    }
}

NetworkCollection::~NetworkCollection()
{
    //dtor
}
