#ifndef LIGHTNETWORK_H
#define LIGHTNETWORK_H

class LightNetwork
{
    public:
        virtual bool doUpdate(bool force) = 0;
        virtual void setShutdown(bool val) = 0;

    protected:
    private:
};

#endif // NETWORK_H
