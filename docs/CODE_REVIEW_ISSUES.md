# GregsLights Code Review — Issues to Fix

Generated: 2026-04-05

---

## Critical Issues

### 1. Missing virtual destructors on base classes
- **`include/controller/LightNetwork.h`** — No virtual destructor. All polymorphic deletion through `LightNetwork*` is undefined behaviour.
  - Fix: add `virtual ~LightNetwork() {}`
- **`include/controller/Bulb.h`** — No virtual destructor. All `FadeableBulb`, `DMXBulb`, `E131Bulb`, `DDPBulb`, `LORBulb`, `DummyBulb` deleted through `Bulb*` is undefined behaviour.
  - Fix: add `virtual ~Bulb() {}`

### 2. FadeableBulb global linked list — data race + use-after-free
- **`src/controller/FadeableBulb.cpp`:16-22** — `firstBulb` static list is mutated from the main thread (constructor) and traversed from the tick thread simultaneously with no mutex.
- Destructor does not unlink `this` from the list, leaving dangling pointers that the tick thread will dereference.
  - Fix: add a mutex protecting `firstBulb`; unlink `this` from the list in the destructor.

### 3. RGBPicture::allData never freed
- **`src/RGBPicture.cpp`:169-176** — `allData` is allocated with `malloc()` but the destructor has `delete allData` commented out with a TODO. `delete` is wrong for `malloc`'d memory; needs `free()`.
  - Fix: replace commented block with `free(allData); allData = nullptr;`

### 4. E131Network — empty destructor leaks socket; wrong fd sentinel
- **`src/controller/E131Network.cpp`**:
  - Destructor is empty — `fd` (UDP socket) is never closed.
  - `fd` initialized to `0` (which is stdin). Should be `-1`.
  - If `bind()` fails, socket from `socket()` call is leaked.
  - Fix: initialize `fd = -1`; add `close(fd)` in destructor; close `fd` before throwing on `bind()` failure.

### 5. DDPOutput — empty destructor leaks socket and heap buffer
- **`src/controller/DDPOutput.cpp`**:
  - Destructor is empty — `fd` (UDP socket) and `data` (`calloc`'d buffer) are never released.
  - Same `fd = 0` sentinel bug as E131Network.
  - Fix: add `close(fd)` and `free(data)` in destructor; initialize `fd = -1`.

### 6. LORNetwork — destructor leaks SerialPort; throw from stack buffer
- **`src/controller/LORNetwork.cpp`:28-31**:
  - Destructor is empty — `serptr` (`SerialPort*`) is never deleted.
  - Exception thrown as pointer to stack-local `char errmsg[100]` — the buffer is destroyed as the stack unwinds; any catch handler that reads the message has undefined behaviour.
  - Fix: `delete serptr` in destructor; use `std::string` or `std::runtime_error` for the exception.
- Same stack-buffer throw pattern exists in **`src/controller/OpenDMXNetwork.cpp`**.

### 7. DisplayModel destructor leaks everything
- **`src/DisplayModel.cpp`:208-212** — Destructor contains only a comment. Never deletes:
  - `mqtt`, `networks`, `networkClock`, `networkSnowman`, `networkP10`
  - `clock`, `sign`, `snowmen`, `garageSign`, `largeGrid`
  - All 15 `E131Network*` and 2 `DDPOutput*` objects
  - `messages[0..9]` (even acknowledged in the comment)
  - Fix: delete all owned pointers; consider using `std::unique_ptr`.

### 8. E131Network — off-by-one allows out-of-bounds write
- **`src/controller/E131Network.cpp`:300** — bounds check is `channel > 512` but valid range is 0–511; `channel == 512` writes to `data[638]` which is one past the end of the array.
  - Fix: change `channel > 512` to `channel >= 512`.
- Same off-by-one in **`src/controller/DDPOutput.cpp`:184,197** (`start > maxPixels` and `channel > maxPixels*3`).

### 9. RGBPicture — FILE handle leaked on PNG check failure
- **`src/RGBPicture.cpp`:203-221** — In `check_if_png()`, if `fread()` returns fewer bytes than expected, the function returns `0` without calling `fclose(fp)`.
  - Fix: call `fclose(fp)` before returning `0`.

### 10. NetworkCollection — background threads never joined
- **`src/controller/NetworkCollection.cpp`:30-31** — `serial_t` and `bulb_t` threads are started in the constructor; the destructor is empty. Threads run `while(1)` with no exit flag. If the `NetworkCollection` is ever destroyed, threads will access freed memory.
  - Fix: add a `bool running` flag (or `std::atomic<bool>`); set to false and join threads in destructor.

### 11. GregMQTT — loop thread not stopped; null payload not checked
- **`src/GregMQTT.cpp`:396-399** — Destructor is empty. Mosquitto's internal loop thread (`loop_start()`) is never stopped with `loop_stop()` / `disconnect()`.
- **`src/GregMQTT.cpp`:127,182** — Several `on_message` branches construct `std::string` from `message->payload` without checking for null, unlike some other branches that do check.
  - Fix: call `loop_stop(true)` and `disconnect()` in destructor; add null check before using `payload`.

### 12. Sign / GarageSign / LargeGrid — thousands of allocated objects never freed
- **`src/Sign.cpp`**, **`src/GarageSign.cpp`**, **`src/LargeGrid.cpp`** — Destructors are empty stubs. Each allocates thousands of `RGBLight` and `DummyBulb` objects via `new` into `pixals[]` / `board[]` arrays that are never deleted.

---

## Important Issues

### 13. TimeInfo — not thread-safe; tm_now is a dangling pointer risk
- **`src/TimeInfo.cpp`:100-106** — `localtime()` is not thread-safe (returns pointer to a shared static buffer). Multiple display threads access `TimeInfo` simultaneously.
- `tm_now` is stored as `struct tm*` pointing into that static buffer, invalidated by any subsequent `localtime()` call from any thread.
  - Fix: use `localtime_r(&t_now, &tm_now)` and store `tm_now` as `struct tm` by value (not a pointer).

### 14. serial_posix.cpp — fd leaked on ioctl failure
- **`src/controller/serial_posix.cpp`:109** — If `ioctl(TIOCEXCL)` or subsequent `tcsetattr` calls fail, the function returns `-1` without calling `close(fd)`.
  - Fix: close `fd` before all early-return error paths.

### 15. FadeableBulb — fade-down never terminates correctly
- **`src/controller/FadeableBulb.cpp`:112** — The stop condition `percentage <= fadeStep` compares brightness (0–100) against a tiny per-tick step value. This is effectively never true. Should be `percentage <= fadeStop`.

### 16. MessageGenerator — dangling pointer from temporary std::string
- **`src/MessageGenerator.cpp`:52-54** — `getStartDate().c_str()` returns a pointer into a temporary `std::string` that is destroyed at end of expression. `strcpy(this->message, s)` then reads freed memory.
- Same pattern at `getMessage()` lines ~80, 108: `return (char*) getStartDate().c_str()` returns a dangling pointer.
  - Fix: store the `std::string` in a local variable before calling `.c_str()`.

### 17. LargeGrid — null pixels warned but then dereferenced
- **`src/LargeGrid.cpp`:100-104** — First loop prints a warning if `pixals[i] == NULL`, but the second loop calls `pixals[i]->set(0,0,0)` unconditionally — guaranteed null dereference crash if any pixel is unmapped.
  - Fix: add a null check in the second loop (or fix the mapping so no nulls are possible).

### 18. NetworkCollection — change counter double-incremented
- **`src/controller/NetworkCollection.cpp`:89-96** — `change` is incremented once unconditionally and then again inside the `if (++change > maxBeforeSleep)` check, so the sleep throttle fires at half the intended threshold.
  - Fix: remove the first `++change` and use only the `if (++change > maxBeforeSleep)` form.

### 19. CountdownClock — special[] array uninitialized
- **`include/CountdownClock.h`:57** — The `special[8]` array of `Bulb*` pointers is never initialized to null in the constructor. `setAllOff()` calls `setIntensity(0)` on each entry. If `setSpecial()` has not been called for all indices, the code dereferences garbage.
  - Fix: zero-initialize the array in the constructor: `memset(special, 0, sizeof(special))` or `special{}` initialization.

### 20. NetworkCollection — strcpy into fixed 80-byte buffer
- **`src/controller/NetworkCollection.cpp`:17** — `strcpy(this->name, name)` with no length check. Buffer is 80 bytes; a longer name overflows.
  - Fix: use `strncpy(this->name, name, sizeof(this->name) - 1); this->name[79] = '\0';`

### 21. RGBPicture — infinite loop if path has no '/'
- **`src/RGBPicture.cpp`:36-40** — Loop decrements `i` looking for `/` with no lower-bound check. A bare filename (no directory component) causes `i` to go negative, accessing `fullname[-1]` — undefined behaviour.
  - Fix: add `i >= 0` to the loop condition.

### 22. FadeableBulb — shared fields written from two threads without synchronization
- **`include/controller/FadeableBulb.h`** — `fadeStep`, `percentage`, `fadeStop` are read by the tick thread and written by display threads with no mutex or `std::atomic`. This is a data race on `double` values.

---

## Minor Issues

### 23. TimeInfo — year calculation off by one
- **`src/TimeInfo.cpp`:65** — `return tm_christmas.tm_year + 1901;` should be `+ 1900` (`tm_year` is years since 1900).

### 24. main.cpp — intentional fire-and-forget new (undocumented)
- **`main.cpp`:96-101** — `new PlainDisplay(model)` / `new TextDisplay(model)` / `new GregsDisplay(model)` are called without storing the pointer. Objects run forever via background threads. This is apparently intentional but is undocumented and prevents clean shutdown.
  - Consider: store the pointer and call a shutdown method from `my_handler`.

### 25. E131Network — duplicate packet byte assignments
- **`src/controller/E131Network.cpp`** — Several bytes in the E1.31 packet initialization block are assigned twice in a row (copy-paste remnants). Harmless but should be cleaned up.

---

## Summary

| Severity | Count |
|---|---|
| Critical | 12 |
| Important | 10 |
| Minor | 3 |

**Highest-impact fixes (do these first):**
1. Add `virtual ~LightNetwork() {}` and `virtual ~Bulb() {}`
2. Fix `FadeableBulb` linked list — add mutex, fix destructor to unlink
3. Fix `RGBPicture::~RGBPicture()` — `free(allData)` not `delete`
4. Fix `E131Network` / `DDPOutput` destructors — close fd, free data, sentinel `fd = -1`
5. Fix `TimeInfo` — use `localtime_r()`, store `struct tm` by value
