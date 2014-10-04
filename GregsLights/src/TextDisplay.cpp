#include "../include/TextDisplay.h"

#include <pthread.h>
#include <curses.h>


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

    if ((mainwin = initscr()) == NULL)
    {
        printf("Error loading ncursur\n");
    }
    else
    {
        noecho();
        cbreak();
        nodelay(mainwin, TRUE);
        keypad(mainwin, TRUE);
        pthread_create(&(this->display_t), NULL, update_display, this);
    }

}

TextDisplay::~TextDisplay()
{
    //dtor
}

void TextDisplay::update()
{
    CountdownClock *clock = model->getClock();
    mvprintw(0, 0, "Clock: %d", clock->getSecondsRemaining());


    //update display
    refresh();
}


