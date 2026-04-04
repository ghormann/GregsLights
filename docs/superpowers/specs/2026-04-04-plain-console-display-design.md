# Plain Console Display Mode Design

**Date:** 2026-04-04
**Status:** Approved

## Overview

Add a `-l` command-line flag that selects a plain console (log) display mode instead of the existing ncurses-based `TextDisplay`. In plain mode, the application polls the same 7 monitored strings every second and prints a prefixed line to stdout whenever a value changes.

## New Class: `PlainDisplay`

### Files
- `GregsLights/include/PlainDisplay.h`
- `GregsLights/src/PlainDisplay.cpp`

### Interface

Define in `PlainDisplay.h`:
```cpp
#define PLAIN_DISPLAY_FIELD_COUNT 7
```

```cpp
class PlainDisplay {
public:
    PlainDisplay(DisplayModel *model);
    virtual ~PlainDisplay();
    void update();
private:
    DisplayModel *model;
    std::string last[PLAIN_DISPLAY_FIELD_COUNT];
    pthread_t display_t;
};
```

### Behavior

- Constructor takes `DisplayModel*`, starts a background pthread (same pattern as `TextDisplay`)
- Thread calls `update()` every 1 second via `sleep(1)`
- `update()` checks each of 7 monitored strings against their last-seen value
- If a value has changed, prints `"<Prefix>: <value>\n"` to stdout and updates last-seen
- No ncurses headers or initialization required

### Monitored Strings

| Prefix | Getter |
|---|---|
| `Display Model 1` | `model->getMessage(1)` |
| `Display Model 2` | `model->getMessage(2)` |
| `Snowmen` | `model->getSnowmen()->getMessage()` |
| `Snowmen Who` | `model->getSnowmen()->getMessageWho()` |
| `Clock` | `model->getClock()->getMessage()` |
| `Sign` | `model->getSign()->getMessage()` |
| `Power` | `model->getGarageSign()->getMessage()` |

## Changes to `main.cpp`

- Add `isLog = FALSE` variable alongside existing `isDebug`, `skip_time_check`, `show_new_year`
- Add `l` and `L` to the `getopt` format string: `"dDtTyYlL"`
- Add `case 'l': case 'L': isLog = TRUE; break;` in the switch
- Update usage string: `"Usage: %s [-d] [-t] [-y] [-l]\n"`
- After `sleep(1)` (allow threads to start), instantiate `PlainDisplay(model)` if `isLog == TRUE`, otherwise `TextDisplay(model)` as today
- Add `#include "include/PlainDisplay.h"`

## Changes to `GregsLights.cbp`

Add two `<Unit>` entries alongside the existing `TextDisplay` entries:

```xml
<Unit filename="include/PlainDisplay.h" />
<Unit filename="src/PlainDisplay.cpp" />
```

## Changes to `README.md`

Add a "Command Line Options" section documenting all four flags:

| Flag | Description |
|---|---|
| `-d` | Debug mode: runs `DisplayTester::testAll()` instead of the normal display loop |
| `-t` | Skip time check: allows the display to run outside of normal scheduled hours |
| `-y` | Show New Year mode |
| `-l` | Plain console (log) mode: prints changed display values to stdout instead of using ncurses |
