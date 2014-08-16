#include "../../include/controller/NetworkCollection.h"

#define MAX_LIGHT_NETWORKS 50

void *update_thread(void *args);


NetworkCollection::NetworkCollection()
{
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        networks[i] = 0;
    }
    pthread_create(&(this->serial_t), NULL, update_thread, (void*) this);

}

void * update_thread(void *args)
{
    NetworkCollection *ptr = (NetworkCollection *)args;
    while (1)
    {
        ptr->doUpdate();
        usleep(50 * 1000); // 50ms
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
