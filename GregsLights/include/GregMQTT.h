#ifndef GREGMQTT_H
#define GREGMQTT_H

#include <mosquittopp.h>
#include <string>

# define GJH_SSL_VERIFY_PEER                 0x01


class GregMQTT : public mosqpp::mosquittopp
{
public:
    GregMQTT(bool enabled);
    virtual ~GregMQTT();
    virtual void on_connect(int rc);
    void on_log(int level, const char *str);
    void sendSignMessage(std::string msg);
    void sendClockMessage(int t);

protected:
    void myPublish(std::string topic, std::string value);

private:
    bool isValid;
    void debug(std::string mesg);
};

#endif // GREGMQTT_H
