#include "GregMQTT.h"
#include <iostream>
#include <sstream>

GregMQTT::GregMQTT(bool enable)
{
    isValid = enable;

    if (enable)
    {
        char *host = "";
        int port = 8883;
        int rc = 0;

        isValid = true;
        mosqpp::lib_init();

        debug("Setting password");
        if (username_pw_set() != MOSQ_ERR_SUCCESS)
        {
            std::cout << "setting passwd failed" << std::endl;
            isValid = false;
        }

        debug("Setting CA");
        if (tls_set("") != MOSQ_ERR_SUCCESS)
        {
            std::cout << "Failed Setting ca" << std::endl;
            isValid = false;
        }

        debug("Setting tls");
        if (tls_opts_set(GJH_SSL_VERIFY_PEER, "tlsv1.2") != MOSQ_ERR_SUCCESS)
        {
            std::cout << "Failed Setting tls version" << std::endl;
            isValid = false;
        }

        debug("Connecting");
        if (isValid)
        {
            rc = connect(host, port);
            if (rc)
            {
                std::cout << "Collect Failed with " << rc << std::endl;
                isValid = false;
            }
        }
        debug("Connection Requested");
        debug("Starting MQTT Background thread");
        rc = loop_start();
        if (rc)
        {
            std::cout << "Call to loop_start failed with: " << rc << std::endl;
            isValid = false;

        }

    }
}

void GregMQTT::on_connect(int rc)
{
    debug("Calling on_connect");
    if (rc)
    {
        std::cout << "MQTT connect Failed with: " << rc << std::endl;
        isValid = false;
    }
    else
    {
    }
}

void GregMQTT::sendClockMessage(int t)
{
    std::ostringstream out;
    out << t;
    this->myPublish("/christmas/clock", out.str());
}

void GregMQTT::sendSignMessage(std::string msg)
{
    this->myPublish("/christmas/sign", msg);
}


void GregMQTT::myPublish(std::string topic, std::string value)
{
    if (!isValid)
    {
        return;
    }
    debug("Starting Publish");
    int ret = publish(NULL, topic.c_str(), value.size(), value.c_str(), 1, false);
    debug("Ending Publish");
    if (ret != MOSQ_ERR_SUCCESS)
    {
        std::cout << "Sending failed with " << ret << std::endl;
    }
}


void GregMQTT::on_log(int level, const char *str)
{
    debug("On_Log:");
    debug(str);
}

void GregMQTT::debug(std::string mesg)
{
    //std::cout << "Debug: " << mesg << std::endl;
}

GregMQTT::~GregMQTT()
{
    //dtor
}
