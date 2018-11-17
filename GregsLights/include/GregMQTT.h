#ifndef GREGMQTT_H
#define GREGMQTT_H

#include <mosquittopp.h>
#include <string>
#include <queue>
#include <mutex>

# define GJH_SSL_VERIFY_PEER                 0x01


class GregMQTT : public mosqpp::mosquittopp
{
public:
    GregMQTT(bool enabled, const char * _id);
    virtual ~GregMQTT();
    virtual void on_connect(int rc);
    void on_message(const struct mosquitto_message *message);
    void on_log(int level, const char *str);
    void sendSignMessage(std::string msg);
    void sendClockMessage(int t);
    std::string getNextName();

protected:
    void myPublish(std::string topic, std::string value);

private:
    bool isValid;
    void debug(std::string mesg);
    std::queue<std::string> name_queue;
    std::mutex name_queue_mutex;
};

#endif // GREGMQTT_H
