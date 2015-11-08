#include "../../include/controller/NetworkCollection.h"
#include "../../include/controller/FadeableBulb.h"
#include <stdio.h>
#include <string.h>

#define MAX_LIGHT_NETWORKS 50

void *update_thread(void *args);


/*
 * msBetween: Millisonds between checking for updates (Tick)
 * maxTicks: Max number of updates skipped before force
 * maxBeforeSleep: Max Universes updated before taking an extra sleep
 * extraSleepMs: Duration (MS) of sleep when MaxBeforeSleep hit.
 */
NetworkCollection::NetworkCollection(char *name, int msBetween, int maxTicks,int maxBeforeSleep, int extraSleepMs)
{
    strcpy(this->name, name);
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        networks[i] = 0;
        counts[i] = 0;
    }
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
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        if (networks[i] != 0)
        {
            force = (counts[i] > maxTicks);
            if (networks[i]->doUpdate(force))
            {
                ++change;
                counts[i] = 0;
                if (++change > maxBeforeSleep)
                {
                    //printf("Sleeping longer for %s: %dms\n", this->name, extraSleepMs);
                    change = 0;
                    usleep(extraSleepMs*1000);
                }
            } else {
                ++(counts[i]);
            }
        }
    }
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
