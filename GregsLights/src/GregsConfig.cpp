#include "../include/GregsConfig.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <json/json.h>

GregsConfig& GregsConfig::getInstance()
{
    static GregsConfig instance;
    return instance;
}

GregsConfig::GregsConfig()
{
    Json::Value root;
    try
    {
        std::ifstream config_doc("/home/ghormann/greglights_config.json", std::ifstream::binary);
        config_doc.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        config_doc >> root;
    }
    catch (std::ifstream::failure const &e)
    {
        throw std::runtime_error(
            std::string("Failed to read greglights_config.json: ") + e.what());
    }

    resourcesPath = root.get("resources_path", "").asString();
    if (resourcesPath.empty())
    {
        throw std::runtime_error(
            "greglights_config.json: 'resources_path' is required but not set");
    }

    mqttUsername = root.get("username", "").asString();
    mqttPassword = root.get("password", "").asString();
    mqttHost     = root.get("host", "").asString();
    mqttPort     = root.get("port", 8883).asInt();
}

const std::string& GregsConfig::getResourcesPath() const { return resourcesPath; }
const std::string& GregsConfig::getMQTTUsername()  const { return mqttUsername; }
const std::string& GregsConfig::getMQTTPassword()  const { return mqttPassword; }
const std::string& GregsConfig::getMQTTHost()      const { return mqttHost; }
int                GregsConfig::getMQTTPort()       const { return mqttPort; }
