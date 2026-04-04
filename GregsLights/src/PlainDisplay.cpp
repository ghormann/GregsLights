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
