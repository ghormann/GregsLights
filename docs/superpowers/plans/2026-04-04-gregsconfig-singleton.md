# GregsConfig Singleton Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace all hardcoded `/home/ghormann/...` resource paths with a `GregsConfig` singleton that reads `greglights_config.json` and throws early on startup if `resources_path` is missing.

**Architecture:** A new `GregsConfig` singleton class reads `greglights_config.json` once at first access. `main()` calls it before constructing `DisplayModel` so config errors surface immediately. `GregMQTT` and `RGBPicture` pull values from the singleton instead of reading the file themselves.

**Tech Stack:** C++11, jsoncpp (`<json/json.h>`), existing build via CodeBlocks `.cbp` / `cbp2make` + `make`

---

## File Map

| File | Action | Responsibility |
|------|--------|----------------|
| `include/GregsConfig.h` | Create | Singleton interface — getters for all config values |
| `src/GregsConfig.cpp` | Create | Reads and validates `greglights_config.json`; throws on missing `resources_path` |
| `main.cpp` | Modify | Call `GregsConfig::getInstance()` early; widen catch to `std::exception` |
| `src/GregMQTT.cpp` | Modify | Replace inline file-reading block with `GregsConfig` getters; remove `<fstream>` |
| `src/RGBPicture.cpp` | Modify | Replace hardcoded dir string with `GregsConfig::getInstance().getResourcesPath()` |
| `src/Sign.cpp` | Modify | Replace hardcoded PNG path in `if(0)` block with `GregsConfig` path |
| `GregsLights.cbp` | Modify | Add `<Unit>` entries for new header and source file |

---

### Task 1: Create `GregsConfig` header

**Files:**
- Create: `GregsLights/include/GregsConfig.h`

- [ ] **Step 1: Create the header file**

```cpp
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
```

- [ ] **Step 2: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/include/GregsConfig.h
git commit -m "feat: add GregsConfig singleton header"
```

---

### Task 2: Create `GregsConfig` implementation

**Files:**
- Create: `GregsLights/src/GregsConfig.cpp`

- [ ] **Step 1: Create the implementation file**

```cpp
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
```

- [ ] **Step 2: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/src/GregsConfig.cpp
git commit -m "feat: implement GregsConfig singleton — reads greglights_config.json"
```

---

### Task 3: Update `main.cpp`

**Files:**
- Modify: `GregsLights/main.cpp`

The goal is to call `GregsConfig::getInstance()` before constructing `DisplayModel` so a bad config aborts immediately with a clear message. Also widen the existing `catch (const char* msg)` to also catch `std::exception`.

- [ ] **Step 1: Add `#include "include/GregsConfig.h"` after the existing includes**

Current includes block (lines 7–14):
```cpp
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "include/DisplayModel.h"
#include "include/DisplayTester.h"
#include "include/TextDisplay.h"
#include "include/GregsDisplay.h"
```

Replace with:
```cpp
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "include/DisplayModel.h"
#include "include/DisplayTester.h"
#include "include/TextDisplay.h"
#include "include/GregsDisplay.h"
#include "include/GregsConfig.h"
```

- [ ] **Step 2: Add early config validation at the top of the `try` block**

Current `try` block opening (line 81):
```cpp
    try
    {
        bool sendDMX = true;
        model = new DisplayModel(sendDMX, skip_time_check, show_new_year );
```

Replace with:
```cpp
    try
    {
        GregsConfig::getInstance(); // validates config; throws std::runtime_error if resources_path missing
        bool sendDMX = true;
        model = new DisplayModel(sendDMX, skip_time_check, show_new_year );
```

- [ ] **Step 3: Add `std::exception` catch before existing `const char*` catch**

Current catch block (lines 104–108):
```cpp
    catch (const char* msg)
    {
        cerr << msg << endl;
        return 1;
    }
```

Replace with:
```cpp
    catch (const std::exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }
    catch (const char* msg)
    {
        cerr << msg << endl;
        return 1;
    }
```

- [ ] **Step 4: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/main.cpp
git commit -m "feat: validate GregsConfig early in main() before DisplayModel init"
```

---

### Task 4: Update `GregMQTT.cpp`

**Files:**
- Modify: `GregsLights/src/GregMQTT.cpp`

Replace the inline file-reading block (lines 24–48) with calls to `GregsConfig`. Keep `<json/json.h>`, `<sstream>`, `<algorithm>` — they are used elsewhere in this file for MQTT message parsing. Remove `<fstream>` since it's no longer needed here.

- [ ] **Step 1: Remove `<fstream>` include and add `GregsConfig.h`**

Current includes (lines 1–8):
```cpp
#include "GregMQTT.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>
#include <string.h>
#include <algorithm>
#include "TimeInfo.h"
```

Replace with:
```cpp
#include "GregMQTT.h"
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <string.h>
#include <algorithm>
#include "TimeInfo.h"
#include "GregsConfig.h"
```

- [ ] **Step 2: Replace the config-reading block**

Current block (lines 20–48):
```cpp
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
        catch (std::ifstream::failure const &e)
        {
            std::cerr << "Exception reading greglights_config.json\n" << std::endl;
            throw e;
        }
```

Replace with:
```cpp
        int rc = 0;
        GregsConfig& cfg = GregsConfig::getInstance();
        std::string username = cfg.getMQTTUsername();
        std::string password = cfg.getMQTTPassword();
        std::string host     = cfg.getMQTTHost();
        int port             = cfg.getMQTTPort();
        if (host.empty() || password.empty() || username.empty())
        {
            isValid = false;
            std::cerr << "Invalid MQTT options in greglights_config.json" << std::endl;
        }
        std::cout << "username: " << username << std::endl;
        std::cout << "host    : " << host << std::endl;
```

- [ ] **Step 3: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/src/GregMQTT.cpp
git commit -m "refactor: GregMQTT reads MQTT config from GregsConfig singleton"
```

---

### Task 5: Update `RGBPicture.cpp`

**Files:**
- Modify: `GregsLights/src/RGBPicture.cpp`

- [ ] **Step 1: Add `#include "../include/GregsConfig.h"` after existing includes**

Current includes (lines 1–6):
```cpp
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>    // std::sort
```

Replace with:
```cpp
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>    // std::sort
#include "../include/GregsConfig.h"
```

- [ ] **Step 2: Replace hardcoded resources dir in `getAllPictures()` (line 128)**

Current line:
```cpp
        std::string dir = std::string("/home/ghormann/Documents/src/gregslights/GregsLights/resources");
```

Replace with:
```cpp
        std::string dir = GregsConfig::getInstance().getResourcesPath();
```

- [ ] **Step 3: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/src/RGBPicture.cpp
git commit -m "refactor: RGBPicture reads resources path from GregsConfig singleton"
```

---

### Task 6: Update `Sign.cpp`

**Files:**
- Modify: `GregsLights/src/Sign.cpp`

The reference is inside a `if(0)` dead-code block at line 829. Still needs updating so the path is no longer hardcoded.

- [ ] **Step 1: Add `#include "../include/GregsConfig.h"` to Sign.cpp includes**

Current includes (lines 1–6):
```cpp
#include "../include/Sign.h"
#include "../include/RGBPicture.h"
#include "../include/controller/DummyBulb.h"
#include "RGBPicture.h"
#include <string>
#include <algorithm>
```

Replace with:
```cpp
#include "../include/Sign.h"
#include "../include/RGBPicture.h"
#include "../include/controller/DummyBulb.h"
#include "RGBPicture.h"
#include "../include/GregsConfig.h"
#include <string>
#include <algorithm>
```

- [ ] **Step 2: Replace hardcoded PNG path at line 829**

Current line:
```cpp
        RGBPicture * picture = new RGBPicture("/home/ghormann/Documents/src/gregslights/GregsLights/resources/orig/test.png");
```

Replace with:
```cpp
        std::string testPngPath = GregsConfig::getInstance().getResourcesPath() + "/orig/test.png";
        RGBPicture * picture = new RGBPicture((char*)testPngPath.c_str());
```

- [ ] **Step 3: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/src/Sign.cpp
git commit -m "refactor: Sign.cpp uses GregsConfig for resources path"
```

---

### Task 7: Update `GregsLights.cbp`

**Files:**
- Modify: `GregsLights/GregsLights.cbp`

- [ ] **Step 1: Add `include/GregsConfig.h` unit — after line 76 (`include/GregMQTT.h`)**

Current lines 76–77:
```xml
		<Unit filename="include/GregMQTT.h" />
		<Unit filename="include/GregsDisplay.h" />
```

Replace with:
```xml
		<Unit filename="include/GregMQTT.h" />
		<Unit filename="include/GregsConfig.h" />
		<Unit filename="include/GregsDisplay.h" />
```

- [ ] **Step 2: Add `src/GregsConfig.cpp` unit — after line 112 (`src/GregMQTT.cpp`)**

Current lines 112–113:
```xml
		<Unit filename="src/GregMQTT.cpp" />
		<Unit filename="src/GregsDisplay.cpp" />
```

Replace with:
```xml
		<Unit filename="src/GregMQTT.cpp" />
		<Unit filename="src/GregsConfig.cpp" />
		<Unit filename="src/GregsDisplay.cpp" />
```

- [ ] **Step 3: Commit**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/GregsLights.cbp
git commit -m "build: add GregsConfig to CodeBlocks project file"
```

---

### Task 8: Build verification

**Files:** None modified — verification only.

- [ ] **Step 1: Regenerate the makefile from the updated `.cbp`**

```bash
cd /home/ghormann/src/GregsLights/GregsLights && cbp2make -in GregsLights.cbp
```

Expected: no errors, `GregsLights.cbp.mak` regenerated.

- [ ] **Step 2: Build the release target**

```bash
cd /home/ghormann/src/GregsLights/GregsLights && make -f GregsLights.cbp.mak release 2>&1 | tail -30
```

Expected: `0 errors` — binary produced at `bin/Release/GregsLights`.

- [ ] **Step 3: Verify no remaining hardcoded paths**

```bash
grep -rn "/home/ghormann/Documents" /home/ghormann/src/GregsLights/GregsLights/src/ /home/ghormann/src/GregsLights/GregsLights/main.cpp
```

Expected: no output.

- [ ] **Step 4: Commit regenerated makefile**

```bash
cd /home/ghormann/src/GregsLights
git add GregsLights/GregsLights.cbp.mak
git commit -m "build: regenerate makefile after adding GregsConfig"
```
