#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include "DisplayModel.h"

class TextDisplay
{
    public:
        TextDisplay(DisplayModel *model);
        virtual ~TextDisplay();
        void update();
    protected:
    private:
        DisplayModel *model;
        pthread_t display_t;      /* Thread for writing to serial interface */
};

#endif // TEXTDISPLAY_H
