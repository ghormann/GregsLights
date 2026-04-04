#ifndef GREGSCONFIG_H
#define GREGSCONFIG_H

#include <string>

class GregsConfig
{
public:
    static GregsConfig& getInstance();

    const std::string& getResourcesPath() const;
    const std::string& getMQTTUsername() const;
    const std::string& getMQTTPassword() const;
    const std::string& getMQTTHost() const;
    int getMQTTPort() const;

private:
    GregsConfig();
    GregsConfig(const GregsConfig&) = delete;
    GregsConfig& operator=(const GregsConfig&) = delete;

    std::string resourcesPath;
    std::string mqttUsername;
    std::string mqttPassword;
    std::string mqttHost;
    int mqttPort;
};

#endif // GREGSCONFIG_H
