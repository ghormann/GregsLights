#include "../include/TextDisplay.h"

#include <pthread.h>
#include <curses.h>


void * update_display(void *args)
{
    TextDisplay *ptr = (TextDisplay *)args;
    while (1)
    {
        ptr->update();
        usleep(100 * 1000); // 100m
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
        curs_set(0);
        nodelay(mainwin, TRUE);
        keypad(mainwin, TRUE);
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_BLACK);

        pthread_create(&(this->display_t), NULL, update_display, this);
    }

}

TextDisplay::~TextDisplay()
{
    //dtor
}

void TextDisplay::update()
{
    clear(); // Clear Warning messages on Screen
    CountdownClock *clock = model->getClock();
    mvprintw(0, 0, "%-60s", model->getMessage(1));
    mvprintw(1, 0, "%-60s", model->getMessage(2));
    mvprintw(2, 0, "Snowmen: %-60s", model->getSnowmen()->getMessage());
    mvprintw(3, 0, "Clock: %-60s", model->getClock()->getMessage());
    mvprintw(4, 0, "Sign: %-60s", model->getSign()->getMessage());
    mvprintw(5, 0, "Power: %-60s", model->getGarageSign()->getMessage());

    mvprintw(0, 64, "Clock: %7d", clock->getSecondsRemaining());


    //update display
    refresh();
}


