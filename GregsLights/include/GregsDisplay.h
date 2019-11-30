#ifndef GREGSDISPLAY_H
#define GREGSDISPLAY_H

#include "DisplayModel.h"
#include "TimeInfo.h"

class GregsDisplay
{
public:
    GregsDisplay(DisplayModel *model);
    virtual ~GregsDisplay();
    void run();
    DisplayModel *getModel();
protected:
private:
    DisplayModel *model;
    TimeInfo *timeinfo;
    static void * signThread(void *);
    static void * snowmenThread(void *);
    static void * mqttThread(void *);
};

#endif // GREGSDISPLAY_H
