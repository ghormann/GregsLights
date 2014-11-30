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
    CountdownClock *clock = model->getClock();
    mvprintw(0, 0, "%s", model->getMessage(1));
    mvprintw(1, 0, "%s", model->getMessage(2));
    mvprintw(2, 0, "Snowmen: %s", model->getSnowmen()->getMessage());
    mvprintw(2, 0, "Clock: %s", model->getClock()->getMessage());

    mvprintw(0, 64, "Clock: %7d", clock->getSecondsRemaining());


    /*
     * Print BUSH
     */
    int line = 9;
    int pos = 0;
    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++)
    {
        Bush *bush = model->getBush(i);
        attrset(A_NORMAL);
        mvprintw(line,pos+1, "Bush %d", i);
        attron(COLOR_PAIR(1));
        mvprintw(line+1, pos, "%3d", bush->getRed()->getIntensity());
        attron(COLOR_PAIR(2));
        mvprintw(line+1, pos+4, "%3d", bush->getGreen()->getIntensity());
        attron(COLOR_PAIR(3));
        mvprintw(line+2, pos, "%3d", bush->getBlue()->getIntensity());
        attrset(A_NORMAL);
        mvprintw(line+2, pos+4, "%3d", bush->getWhite()->getIntensity());

        pos+= 12;
        if (i == 3)
        {
            pos += 5;
        }

    }

    /*
     * Print House
     */
    line = 14;
    pos = 0;
    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        RGBLight *light = model->getHouse(i);
        mvprintw(line,pos+3, "House %d", i);
        attron(COLOR_PAIR(1));
        mvprintw(line+1, pos, "%3d", light->getRed());
        attron(COLOR_PAIR(2));
        mvprintw(line+1, pos+4, "%3d", light->getGreen());
        attron(COLOR_PAIR(3));
        mvprintw(line+1, pos+8, "%3d", light->getBlue());
        attrset(A_NORMAL);

        pos+= 14;
        if (i == 2)
        {
            pos += 10;
        }

    }

    //update display
    refresh();
}


