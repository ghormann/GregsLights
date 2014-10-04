#include "../include/GregsDisplay.h"
#include "../include/controller/Bush.h"

#define GD_RED 1
#define GD_GREEN 2
#define GD_BLUE 3
#define GD_WHITE 4

GregsDisplay::GregsDisplay(DisplayModel *m)
{
    this->model = m;
}

GregsDisplay::~GregsDisplay()
{
    //dtor
}

void GregsDisplay::run() {
    while(1) {
        setAllOff();
        sleep(1);
        set_bush(1,GD_RED,50);
        sleep(1);
    }
}

void GregsDisplay::setAllOff() {
    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++) {
        model->getBush(i)->setAll(0);
    }

    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++) {
        model->getHouse(i)->turnOff();
    }

}

void GregsDisplay::set_bush(int bushId, int color, int intensity) {
    Bush *bush = model->getBush(bushId);
    if (color == GD_RED) {
        bush->getRed()->setIntensity(intensity);
    } else if (color == GD_GREEN) {
        bush->getGreen()->setIntensity(intensity);
    } else if (color == GD_GREEN) {
        bush->getBlue()->setIntensity(intensity);
    } else if (color == GD_WHITE) {
        bush->getWhite()->setIntensity(intensity);
    }
}

