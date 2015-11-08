#include "../../include/controller/NetworkCollection.h"
#include "../../include/controller/FadeableBulb.h"
#include <stdio.h>
#include <string.h>

#define MAX_LIGHT_NETWORKS 50

void *update_thread(void *args);


NetworkCollection::NetworkCollection(char *name)
{
    strcpy(this->name, name);
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        networks[i] = 0;
    }
    this->sleepDuration = 50;   // Duration of sleep if too may updates (in ms)
    this->maxBeforeSleep = MAX_LIGHT_NETWORKS;   //Number of universers to update before extra sleep;

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
        usleep(5 * 1000); // 5ms
    }

    return NULL;
}

/*
 * ALpha Pix has problems if too many updates in short period of time.
 */
void NetworkCollection::setControllerLimits(int maxUpdates, int sleepMS)
{
    this->maxBeforeSleep = maxUpdates;
    this->sleepDuration = sleepMS;
}


void NetworkCollection::doUpdate()
{
    int i = 0;
    int change = 0;
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++)
    {
        if (networks[i] != 0)
        {
            if (networks[i]->doUpdate())
            {
                ++change;
                if (++change > maxBeforeSleep)
                {
                    printf("Sleeping longer for %s\n", this->name);
                    change = 0;
                    usleep(sleepDuration*1000);
                }
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
    sleep(2); // Time for thread to catch up;
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
