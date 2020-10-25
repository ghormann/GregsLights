#ifndef GREGMQTT_H
#define GREGMQTT_H

#include <mosquittopp.h>
#include <string>
#include <deque>
#include <mutex>


# define GJH_SSL_VERIFY_PEER                 0x01


class PowerCallbackInterface
{
public:
    virtual void setPowerCallback(double power) = 0;
    virtual void setCurrentPowerCallback(double dollars, double kwh) = 0;
};

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
    void sendTimeInfo();
    void setPowerCallback(PowerCallbackInterface *ptr);
    int getSnowmanVote();
    std::string getNextName();

protected:
    void myPublish(std::string topic, std::string value);

private:
    bool isValid;
    int snowmanVote = 0;
    PowerCallbackInterface *powerCallback;
    void debug(std::string mesg);
    std::deque<std::string> name_queue;
    std::mutex name_queue_mutex;

};

#endif // GREGMQTT_H
