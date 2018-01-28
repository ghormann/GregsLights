#include "GregMQTT.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>


GregMQTT::GregMQTT(bool enable)
{
    isValid = enable;

    if (enable)
    {
        isValid = true;
        int port = 8883;
        int rc = 0;
        std::string ca_file, username, password, host;

        Json::Value root;
        try
        {
            std::ifstream config_doc("/home/ghormann/greglights_config.json", std::ifstream::binary);
            config_doc.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
            config_doc >> root;
            ca_file = root.get("ca_file", "" ).asString();
            username = root.get("username", "" ).asString();
            password = root.get("password", "" ).asString();
            host = root.get("host", "" ).asString();
            port = root.get("port", "8883" ).asInt();
            if (host.length() == 0 || password.length() == 0 || username.length() == 0 || ca_file.length() ==0)
            {
                isValid = false;
                std::cerr << "Invalid options in Json file" << std::endl;
            }
            std::cout << "CA File : " << ca_file << std::endl;
            std::cout << "username: " << username << std::endl;
            std::cout << "host    : " << host <<  std::endl;
        }
        catch (std::ifstream::failure e)
        {
            std::cerr << "Exception reading greglist_config.json\n" << std::endl;
            throw e;
        }



        mosqpp::lib_init();

        debug("Setting password");
        if (username_pw_set(username.c_str(), password.c_str()) != MOSQ_ERR_SUCCESS)
        {
            std::cout << "setting passwd failed" << std::endl;
            isValid = false;
        }

        debug("Setting CA");
        if (tls_set(ca_file.c_str()) != MOSQ_ERR_SUCCESS)
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
            rc = connect(host.c_str(), port);
            if (rc)
            {
                std::cout << "Connect Failed with " << rc << std::endl;
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
