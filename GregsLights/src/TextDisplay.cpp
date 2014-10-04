#include "../include/TextDisplay.h"

#include <pthread.h>


void * update_display(void *args)
{
    TextDisplay *ptr = (TextDisplay *)args;
    while (1)
    {
        ptr->update();
        usleep(500 * 1000); // 500m
    }

    return NULL;
}


TextDisplay::TextDisplay(DisplayModel *m)
{
    //ctor
    this->model = m;
    pthread_create(&(this->display_t), NULL, update_display, this);

}

TextDisplay::~TextDisplay()
{
    //dtor
}

void TextDisplay::update()
{
    CountdownClock *clock = model->getClock();
    printf("%d\n", clock->getSecondsRemaining());
}


