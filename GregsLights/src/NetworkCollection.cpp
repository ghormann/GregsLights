#include "../include/NetworkCollection.h"

#define MAX_LIGHT_NETWORKS 50

NetworkCollection::NetworkCollection()
{
    for (int i = 0; i < MAX_LIGHT_NETWORKS; i++) {
        networks[i] = 0;
    }
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
