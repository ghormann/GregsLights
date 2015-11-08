#include "../../include/controller/NetworkCollection.h"
#include "../../include/controller/FadeableBulb.h"

#define MAX_LIGHT_NETWORKS 50

void *update_thread(void *args);


NetworkCollection::NetworkCollection()
{
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        networks[i] = 0;
    }
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
        usleep(5 * 1000); // 50ms
    }

    return NULL;
}


void NetworkCollection::doUpdate() {
    int i = 0;
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        if (networks[i] != 0) {
            networks[i]->doUpdate();
        }
    }

}

void NetworkCollection::doShutdown(){
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        if (networks[i]!= 0) {
            networks[i]->setShutdown(true);
        }
    }
    sleep(2); // Time for thread to catch up;
}

void NetworkCollection::removeNetwork( LightNetwork *net) {
    int i = 0;
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        if (networks[i] == net) {
            networks[i] = 0;
            break;
        }
    }
}


void NetworkCollection::addNetwork( LightNetwork *net) {
    int i = 0;
    int added = 0;
    for (i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        if (networks[i] == 0) {
            networks[i] = net;
            added = 1;
            break;
        }
    }
    if (! added) {
        throw "ERROR: NetworkCollection is Full";
    }
}

NetworkCollection::~NetworkCollection()
{
    //dtor
}
