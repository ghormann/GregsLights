#ifndef GREGSDISPLAY_H
#define GREGSDISPLAY_H

#include "DisplayModel.h";

class GregsDisplay
{
    public:
        GregsDisplay(DisplayModel *model);
        virtual ~GregsDisplay();
        void run();
    protected:
    private:
        DisplayModel *model;
        void set_bush(int bush, int color, int intensity);
        void setAllOff();
;
};

#endif // GREGSDISPLAY_H
