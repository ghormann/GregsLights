#ifndef PLAINDISPLAY_H
#define PLAINDISPLAY_H

#include "DisplayModel.h"
#include <string>
#include <pthread.h>

/* getMessage(1..2), snowmen msg/who, clock, sign, garage sign */
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
        pthread_t display_t;      /* Thread for polling display updates */
};

#endif // PLAINDISPLAY_H
