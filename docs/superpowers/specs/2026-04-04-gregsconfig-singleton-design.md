# GregsConfig Singleton Design

**Date:** 2026-04-04
**Topic:** Replace hardcoded resource paths with config-driven `GregsConfig` singleton

## Problem

Several `.cpp` files contain hardcoded references to the developer's local filesystem path:
- `src/RGBPicture.cpp:128` — resources directory used by `getAllPictures()`
- `src/Sign.cpp:829` — full path to a test PNG (inside a dead `if(0)` block)
- `src/GregMQTT.cpp:27` — reads `greglights_config.json` independently

This makes the project non-portable and the config reading is duplicated/fragmented.

## Design

### New: `GregsConfig` Singleton

**Files:** `include/GregsConfig.h`, `src/GregsConfig.cpp`

A singleton class that reads `/home/ghormann/greglights_config.json` once and exposes all configuration values via getters. Throws `std::runtime_error` during construction if `resources_path` is missing or empty.

```cpp
class GregsConfig {
public:
    static GregsConfig& getInstance();

    const std::string& getResourcesPath() const;
    const std::string& getMQTTUsername() const;
    const std::string& getMQTTPassword() const;
    const std::string& getMQTTHost() const;
    int getMQTTPort() const;

private:
    GregsConfig();  // reads and validates greglights_config.json; throws on missing resources_path
    std::string resourcesPath;
    std::string mqttUsername;
    std::string mqttPassword;
    std::string mqttHost;
    int mqttPort;
};
```

The JSON key for the resources path is `"resources_path"`.

### `greglights_config.json` — new required field

```json
{
  "resources_path": "/home/ghormann/Documents/src/gregslights/GregsLights/resources",
  "username": "...",
  "password": "...",
  "host": "...",
  "port": 8883
}
```

### `main.cpp`

Call `GregsConfig::getInstance()` at the top of `main()`, before constructing `DisplayModel`. This ensures a missing or malformed config is caught immediately on startup before any hardware/network initialization.

### `src/GregMQTT.cpp`

Remove the inline `ifstream` / `Json::Value` config-reading block. Replace with calls to `GregsConfig::getInstance()` getters. The existing error handling (setting `isValid = false` on bad values) is preserved using the same validation logic.

### `src/RGBPicture.cpp`

In `getAllPictures()`, replace:
```cpp
std::string dir = std::string("/home/ghormann/Documents/src/gregslights/GregsLights/resources");
```
with:
```cpp
std::string dir = GregsConfig::getInstance().getResourcesPath();
```

### `src/Sign.cpp:829`

Inside the `if(0)` dead-code block, replace the hardcoded path:
```cpp
new RGBPicture("/home/ghormann/.../resources/orig/test.png")
```
with:
```cpp
new RGBPicture((char*)(GregsConfig::getInstance().getResourcesPath() + "/orig/test.png").c_str())
```

### `GregsLights.cbp`

Add two `<Unit>` entries in alphabetical position:
- `<Unit filename="include/GregsConfig.h" />` — after `include/GregMQTT.h`
- `<Unit filename="src/GregsConfig.cpp" />` — after `src/GregMQTT.cpp`

## Error Handling

- If `resources_path` is absent or empty in the JSON, `GregsConfig` constructor throws `std::runtime_error` with a clear message.
- `main.cpp` catches this (via the existing `catch (const char* msg)` — promote to `catch (const std::exception& e)`) and prints the error before exiting.
- All other missing MQTT fields retain existing behavior (`isValid = false` + `cerr` message in `GregMQTT`).

## Files Changed

| File | Change |
|------|--------|
| `include/GregsConfig.h` | New file |
| `src/GregsConfig.cpp` | New file |
| `main.cpp` | Add early `GregsConfig::getInstance()` call; widen catch to `std::exception` |
| `src/GregMQTT.cpp` | Remove file reading, use `GregsConfig` getters |
| `src/RGBPicture.cpp` | Use `GregsConfig::getInstance().getResourcesPath()` |
| `src/Sign.cpp` | Use `GregsConfig` path in dead-code block |
| `GregsLights.cbp` | Add two `<Unit>` entries |
