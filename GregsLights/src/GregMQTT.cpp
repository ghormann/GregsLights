#include "GregMQTT.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>
#include <string.h>
#include <algorithm>
#include "TimeInfo.h"


GregMQTT::GregMQTT(bool enable, const char * _id) : mosquittopp(_id)
{
    isValid = enable;

    if (enable)
    {
        isValid = true;
        int port = 8883;
        int rc = 0;
        std::string username, password, host;

        Json::Value root;
        try
        {
            std::ifstream config_doc("/home/ghormann/greglights_config.json", std::ifstream::binary);
            config_doc.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
            config_doc >> root;
            //ca_file = root.get("ca_file", "" ).asString();
            username = root.get("username", "" ).asString();
            password = root.get("password", "" ).asString();
            host = root.get("host", "" ).asString();
            port = root.get("port", "8883" ).asInt();
            if (host.length() == 0 || password.length() == 0 || username.length() == 0  /*|| ca_file.length() ==0*/ )
            {
                isValid = false;
                std::cerr << "Invalid options in Json file" << std::endl;
            }
            //std::cout << "CA File : " << ca_file << std::endl;
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
        /*
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
        */

        debug("Connecting");
        if (isValid)
        {
            rc = connect(host.c_str(), port, 120);
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

int GregMQTT::getSnowmanVote() {
    if (++snowmanVote > 3) {
        snowmanVote = 0;
    }
    return snowmanVote;
}

std::string GregMQTT::getNextName()
{
    // Mutext clearned when block ends
    std::lock_guard<std::mutex> lock(name_queue_mutex);
    if (this->name_queue.size() > 0)
    {
        std::string name = this->name_queue.front();
        this->name_queue.pop_front();
        return name;
    }
    return std::string();
}

void GregMQTT::on_message(const struct mosquitto_message *message)
{
    if (strcmp("/christmas/clock/setNoShow", message->topic) ==0)
    {
        if (message->payload != NULL)
        {
            std::string data = std::string(reinterpret_cast<char*>(message->payload));
            std::for_each(data.begin(), data.end(), [](char & c)
            {
                c = ::toupper(c);
            });
            bool newMode = false;
            if (data.compare("TRUE") == 0)
            {
                newMode = true;
            }
            TimeInfo::getInstance()->setNoShow(newMode);
            printf("Setting noShow to %s becasuse of %s\n", (newMode? "True" : "False"), data.c_str());
        }
    }
    else if (strcmp("/christmas/todayPower", message->topic) ==0 )
    {
        std::string strJson = std::string(reinterpret_cast<char*>(message->payload));
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( strJson.c_str(), root );     //parse process
        if ( !parsingSuccessful )
        {
            std::cout  << "Failed to parse todayPower"
                       << reader.getFormattedErrorMessages()
                       << std::endl;
        }
        else
        {
            double kwh = root.get("kwh", 0.0 ).asDouble();
            double dollars = root.get("dollars", 0.0).asDouble();
            if (powerCallback)
            {
                powerCallback->setCurrentPowerCallback(dollars, kwh);
            }
            else
            {
                std::cout << "Now Power Callback! Power: " << std::endl;
            }
        }
    }
    else if (strncmp("/christmas/power/", message->topic,17) ==0 )
    {
        std::string strJson = std::string(reinterpret_cast<char*>(message->payload));
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( strJson.c_str(), root );     //parse process
        if ( !parsingSuccessful )
        {
            std::cout  << "Failed to parse"
                       << reader.getFormattedErrorMessages()
                       << std::endl;
        }
        else
        {
            double power = 0;
            for (Json::Value::iterator it=root.begin(); it!=root.end(); ++it)
            {
                power += it->asDouble();
            }
            if (powerCallback)
            {
                powerCallback->setPowerCallback(power);
            }
            else
            {
                std::cout << "Now Power Callback! Power: " << power << std::endl;
            }
        }

    }
    else if (strcmp("/christmas/clock/setTimeCheck", message->topic) ==0)
    {
        if (message->payload != NULL)
        {
            std::string data = std::string(reinterpret_cast<char*>(message->payload));
            std::for_each(data.begin(), data.end(), [](char & c)
            {
                c = ::toupper(c);
            });
            bool newMode = false;
            if (data.compare("TRUE") == 0)
            {
                newMode = true;
            }
            TimeInfo::getInstance()->setSkipTimeCheck(newMode);
            printf("Setting Timecheck to %s becasuse of %s\n", (newMode? "True" : "False"), data.c_str());
        }
    }
    else if (strcmp("/christmas/clock/setDebug", message->topic) ==0)
    {
        if (message->payload != NULL)
        {
            std::string data = std::string(reinterpret_cast<char*>(message->payload));
            std::for_each(data.begin(), data.end(), [](char & c)
            {
                c = ::toupper(c);
            });
            bool newMode = false;
            if (data.compare("TRUE") == 0)
            {
                newMode = true;
            }
            TimeInfo::getInstance()->setDebug(newMode);
            printf("Setting Debug to %s becasuse of %s\n", (newMode? "True" : "False"), data.c_str());
        }
    }
    else if (
        (strcmp("/christmas/personsName", message->topic) == 0)
        || (strcmp("/christmas/personsNameFront", message->topic) == 0)
        || (strcmp("/christmas/personsNameLow", message->topic) == 0)
    )
    {
        std::string strJson = std::string(reinterpret_cast<char*>(message->payload));
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( strJson.c_str(), root );     //parse process
        if ( !parsingSuccessful )
        {
            std::cout  << "Failed to parse"
                       << reader.getFormattedErrorMessages()
                       << std::endl;
        }
        else
        {

            std::string name = root.get("name", "NO NAME" ).asString();
            std::transform(name.begin(), name.end(),name.begin(), ::toupper);
            // Push name only if not in queue already
            std::lock_guard<std::mutex> lock(name_queue_mutex);
            auto it = std::find(name_queue.begin(), name_queue.end(), name);
            if (it == name_queue.end())
            {
                this->name_queue.push_back(name);
            }
        }
    }
}

void GregMQTT::setPowerCallback(PowerCallbackInterface *ptr)
{
    this->powerCallback = ptr;
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
        std::cout << "Subscribing" << std::endl;
        subscribe(NULL, "/christmas/personsName");
        subscribe(NULL, "/christmas/personsNameFront");
        subscribe(NULL, "/christmas/personsNameLow");
        subscribe(NULL, "/christmas/clock/setNoShow");
        subscribe(NULL, "/christmas/clock/setTimeCheck");
        subscribe(NULL, "/christmas/clock/setDebug");
        subscribe(NULL, "/christmas/power/#");
        subscribe(NULL, "/christmas/todayPower");
    }
}

void GregMQTT::sendClockMessage(int t)
{
    std::ostringstream out;
    out << t;
    this->myPublish("/christmas/clock", out.str());
}

void GregMQTT::sendTimeInfo()
{
    TimeInfo *ti = TimeInfo::getInstance();
    Json::Value obj;
    obj["skipTime"] = ti->isSkipTimeCheck();
    obj["debug"] = ti->isDebug();
    obj["newYears"] = ti->isNewYears();
    obj["noShow"] = ti->isNoShow();
    obj["displayHours"] = ti->isDisplayHours();

    Json::StreamWriterBuilder builder;
    builder["indentation"] = ""; // If you want whitespace-less output
    const std::string output = Json::writeString(builder, obj);
    this->myPublish("/christmas/timeinfo", output.c_str());
}

void GregMQTT::sendSignMessage(const std::string& msg)
{
    this->myPublish("/christmas/sign", msg);
}

void GregMQTT::sendSnowmanJson(const std::string& msg) {
    this->myPublish("/christmas/snowman/status", msg);
}



void GregMQTT::myPublish(std::string topic, std::string value)
{
    if (!isValid)
    {
        return;
    }
    debug("Starting Publish");
    debug(topic);
    debug(value);
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
