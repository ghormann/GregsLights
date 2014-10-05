#ifndef GREGSDISPLAY_H
#define GREGSDISPLAY_H

#include "DisplayModel.h"

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
        void fade_bush(int bush, int color, int s_intensity, int f_intensity, double duration);
        void setAllOff();
        void rotate_some();
;
};

#endif // GREGSDISPLAY_H
