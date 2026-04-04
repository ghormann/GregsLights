# Plain Console Display Mode Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a `-l` flag that runs a plain console display mode (no ncurses) that prints changed display values as prefixed lines to stdout, polling every second.

**Architecture:** A new `PlainDisplay` class mirrors the existing `TextDisplay` structure — constructor takes `DisplayModel*`, spawns a background pthread, and calls `update()` every second. `update()` compares 7 monitored strings to their last-seen values and prints any that changed. `main.cpp` instantiates `PlainDisplay` instead of `TextDisplay` when `-l` is passed.

**Tech Stack:** C++, pthreads, `std::string` for last-seen comparison, no ncurses dependency.

---

## File Map

| Action | File | Purpose |
|---|---|---|
| Create | `GregsLights/include/PlainDisplay.h` | Class declaration, `PLAIN_DISPLAY_FIELD_COUNT` define |
| Create | `GregsLights/src/PlainDisplay.cpp` | Constructor, pthread loop, `update()` implementation |
| Modify | `GregsLights/main.cpp` | Add `-l`/`-L` flag, `isLog` variable, `PlainDisplay` include and instantiation |
| Modify | `GregsLights/GregsLights.cbp` | Add `<Unit>` entries for new files |
| Regenerate | `GregsLights/GregsLights.cbp.mak` | Regenerate from `.cbp` using `cbp2make` |
| Modify | `README.md` | Add Command Line Options section |

---

### Task 1: Create `PlainDisplay.h`

**Files:**
- Create: `GregsLights/include/PlainDisplay.h`

- [ ] **Step 1: Create the header file**

```cpp
#ifndef PLAINDISPLAY_H
#define PLAINDISPLAY_H

#include "DisplayModel.h"
#include <string>
#include <pthread.h>

#define PLAIN_DISPLAY_FIELD_COUNT 7

class PlainDisplay
{
    public:
        PlainDisplay(DisplayModel *model);
        virtual ~PlainDisplay();
        void update();
    protected:
    private:
        DisplayModel *model;
        std::string last[PLAIN_DISPLAY_FIELD_COUNT];
        pthread_t display_t;
};

#endif // PLAINDISPLAY_H
```

- [ ] **Step 2: Verify it compiles in isolation**

Run from `GregsLights/`:
```bash
g++ -std=c++0x -Iinclude -Iinclude/controller -c /dev/null -include include/PlainDisplay.h -o /dev/null
```
Expected: No errors or warnings.

- [ ] **Step 3: Commit**

```bash
git add GregsLights/include/PlainDisplay.h
git commit -m "feat: add PlainDisplay header with PLAIN_DISPLAY_FIELD_COUNT define"
```

---

### Task 2: Create `PlainDisplay.cpp`

**Files:**
- Create: `GregsLights/src/PlainDisplay.cpp`

- [ ] **Step 1: Create the implementation**

```cpp
#include "../include/PlainDisplay.h"
#include <stdio.h>
#include <unistd.h>

static void * update_plain_display(void *args)
{
    PlainDisplay *ptr = (PlainDisplay *)args;
    while (1)
    {
        ptr->update();
        sleep(1);
    }
    return NULL;
}

PlainDisplay::PlainDisplay(DisplayModel *m)
{
    this->model = m;
    pthread_create(&(this->display_t), NULL, update_plain_display, this);
}

PlainDisplay::~PlainDisplay()
{
}

void PlainDisplay::update()
{
    const char *current[PLAIN_DISPLAY_FIELD_COUNT];
    current[0] = model->getMessage(1);
    current[1] = model->getMessage(2);
    current[2] = model->getSnowmen()->getMessage();
    current[3] = model->getSnowmen()->getMessageWho();
    current[4] = model->getClock()->getMessage();
    current[5] = model->getSign()->getMessage();
    current[6] = model->getGarageSign()->getMessage();

    const char *prefixes[PLAIN_DISPLAY_FIELD_COUNT] = {
        "Display Model 1",
        "Display Model 2",
        "Snowmen",
        "Snowmen Who",
        "Clock",
        "Sign",
        "Power"
    };

    for (int i = 0; i < PLAIN_DISPLAY_FIELD_COUNT; i++)
    {
        std::string val(current[i] ? current[i] : "");
        if (val != last[i])
        {
            printf("%s: %s\n", prefixes[i], val.c_str());
            last[i] = val;
        }
    }
}
```

- [ ] **Step 2: Compile the file directly to verify no errors**

Run from `GregsLights/`:
```bash
g++ -std=c++0x -Iinclude -Iinclude/controller -I/usr/include/jsoncpp -I/usr/include/GraphicsMagick -c src/PlainDisplay.cpp -o /tmp/PlainDisplay.o
```
Expected: No errors or warnings.

- [ ] **Step 3: Commit**

```bash
git add GregsLights/src/PlainDisplay.cpp
git commit -m "feat: implement PlainDisplay - plain console mode polling every second"
```

---

### Task 3: Update `main.cpp`

**Files:**
- Modify: `GregsLights/main.cpp`

- [ ] **Step 1: Add `isLog` variable, update getopt string and usage**

In `main.cpp`, change:
```cpp
    int skip_time_check = FALSE;
    int show_new_year = FALSE;
    int isDebug = FALSE;
    int ch;
```
to:
```cpp
    int skip_time_check = FALSE;
    int show_new_year = FALSE;
    int isDebug = FALSE;
    int isLog = FALSE;
    int ch;
```

Change the getopt call:
```cpp
    while ((ch = getopt(argc, argv, "dDtTyY")) != -1)
```
to:
```cpp
    while ((ch = getopt(argc, argv, "dDtTyYlL")) != -1)
```

Add cases to the switch (before `case '?'`):
```cpp
        case 'l':
        case 'L':
            isLog = TRUE;
            break;
```

Update the usage string:
```cpp
            printf("Usage: %s [-d] [-t] [-y] [-l]\n", argv[0]);
```

- [ ] **Step 2: Add include and update display instantiation**

Add to includes at top of `main.cpp` (after `#include "include/TextDisplay.h"`):
```cpp
#include "include/PlainDisplay.h"
```

Change:
```cpp
        new TextDisplay(model);
```
to:
```cpp
        if (isLog == TRUE)
            new PlainDisplay(model);
        else
            new TextDisplay(model);
```

- [ ] **Step 3: Build debug target to verify compilation**

Run from `GregsLights/`:
```bash
g++ -std=c++0x -Iinclude -Iinclude/controller -I/usr/include/jsoncpp -I/usr/include/GraphicsMagick -c main.cpp -o /tmp/main.o
```
Expected: No errors.

- [ ] **Step 4: Commit**

```bash
git add GregsLights/main.cpp
git commit -m "feat: add -l flag to select PlainDisplay instead of ncurses TextDisplay"
```

---

### Task 4: Update `GregsLights.cbp` and regenerate Makefile

**Files:**
- Modify: `GregsLights/GregsLights.cbp`
- Regenerate: `GregsLights/GregsLights.cbp.mak`

- [ ] **Step 1: Add `PlainDisplay` unit entries to `GregsLights.cbp`**

In `GregsLights/GregsLights.cbp`, after the line:
```xml
		<Unit filename="include/TextDisplay.h" />
```
add:
```xml
		<Unit filename="include/PlainDisplay.h" />
```

After the line:
```xml
		<Unit filename="src/TextDisplay.cpp" />
```
add:
```xml
		<Unit filename="src/PlainDisplay.cpp" />
```

- [ ] **Step 2: Regenerate the Makefile**

Run from `GregsLights/`:
```bash
cbp2make -in GregsLights.cbp
```
Expected: `GregsLights.cbp.mak` is updated. Verify `PlainDisplay` appears in the output:
```bash
grep PlainDisplay GregsLights.cbp.mak
```
Expected: Lines referencing `src/PlainDisplay.cpp` and `PlainDisplay.o` appear in OBJ_DEBUG, OBJ_RELEASE, and OBJ_GRAPHICS lists, plus compile rules.

- [ ] **Step 3: Build release to verify full link succeeds**

Run from `GregsLights/`:
```bash
make -f GregsLights.cbp.mak release 2>&1 | tail -20
```
Expected: Build completes with `bin/Release/GregsLights` produced, no errors.

- [ ] **Step 4: Commit**

```bash
git add GregsLights/GregsLights.cbp GregsLights/GregsLights.cbp.mak
git commit -m "build: add PlainDisplay to CodeBlocks project and regenerate Makefile"
```

---

### Task 5: Update `README.md`

**Files:**
- Modify: `README.md`

- [ ] **Step 1: Add Command Line Options section**

In `README.md`, after the `### Contributions ###` heading (or before it, so options appear before contributions), add:

```markdown
### Command Line Options ###

| Flag | Description |
|------|-------------|
| `-d` | Debug mode: runs `DisplayTester::testAll()` instead of the normal display loop |
| `-t` | Skip time check: allows the display to run outside of normal scheduled hours |
| `-y` | Show New Year mode |
| `-l` | Plain console (log) mode: prints changed display values to stdout instead of using ncurses |
```

- [ ] **Step 2: Commit**

```bash
git add README.md
git commit -m "docs: add Command Line Options section to README"
```

---

## Manual Verification

After all tasks are complete, verify the feature end-to-end:

1. Run with `-l` flag: `sudo bin/Release/GregsLights -l -t`
2. Confirm: No ncurses screen initializes — terminal remains in normal mode
3. Confirm: As display values change, prefixed lines appear in the console, e.g.:
   ```
   Clock: 3d 12:34:56
   Snowmen: Greg vs Robin
   Sign: Merry Christmas
   ```
4. Confirm: Unchanged values are not reprinted on every tick
5. Run without `-l`: `sudo bin/Release/GregsLights -t`
6. Confirm: ncurses display initializes as normal
