#ifndef NETWORKCOLLECTION_H
#define NETWORKCOLLECTION_H

#define MAX_LIGHT_NETWORKS 50
#include "LightNetwork.h"


/**
 * Tracks the different networks that have been added for the purpose
 * of preforming updates.
*/

class NetworkCollection
{
    public:
        NetworkCollection();
        virtual ~NetworkCollection();
        void addNetwork( LightNetwork *net);
        void removeNetwork ( LightNetwork *net);
        void doUpdate();
    protected:
    private:
        LightNetwork* networks[50];
};

#endif // NETWORKCOLLECTION_H
