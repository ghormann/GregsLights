#include "../include/GregsDisplay.h"
#include "../include/controller/Bush.h"

#define GD_RED 0
#define GD_GREEN 1
#define GD_BLUE 2
#define GD_WHITE 3


GregsDisplay::GregsDisplay(DisplayModel *m)
{
    this->model = m;
}

GregsDisplay::~GregsDisplay()
{
    //dtor
}

void GregsDisplay::run()
{
    while(1)
    {
        setAllOff();
        sleep(1);
        rotate_some();
    }
}

void GregsDisplay:: rotate_some()
{
    int i;
    int j;
    int color;
    int start=1;
    double duration=0.3;

    model->getHouse(1)->fade(0,0,0,100,0,0,10);
    model->getHouse(2)->fade(0,0,0,0,0,100,10);
    model->getHouse(3)->fade(0,0,0,0,0,100,10);
    model->getHouse(4)->fade(0,0,0,0,100,0,10);

    while(start <= 6)
    {
        for (j = 0; j < 2; j++)
        {
            for (color=GD_RED; color<=GD_WHITE; color++)
            {
                sprintf(model->getMessage(1),"Rotate some bushes starting with: %i", start);
                for (i = start; i <= 6; i++)
                {
                    set_bush(i, color, 100);
                }
                write_data(duration);
                for (i = start; i <= 6; i++)
                {
                    set_bush(i, color, 0);
                }
                write_data(0.1);
            }
        }
        set_bush(start, (start-1)%4, 100);
        start++;
    }
    write_data(3);

    for (i = 1; i<=6; i++)
    {
        fade_bush(i, (i-1)%4, 100,0,4.0);
    }

    model->getHouse(1)->fade(100,0,0,0,0,0,10);
    model->getHouse(2)->fade(0,0,100,0,0,0,10);
    model->getHouse(2)->fade(0,0,100,0,0,0,10);
    model->getHouse(3)->fade(0,100,0,0,0,0,10);

    write_data(4.0);

}

void GregsDisplay::setAllOff()
{
    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++)
    {
        model->getBush(i)->setAll(0);
    }

    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        model->getHouse(i)->turnOff();
    }
}

void GregsDisplay::fade_bush(int bushId, int color, int s_intensity, int f_intensity, double duration)
{
    if (bushId < BUSH_LIGHT_START || bushId > BUSH_LIGHT_END) {
        throw "Illegal Bush ID";
    }
    Bush *bush = model->getBush(bushId);
    Bulb *bulb;
    switch(color)
    {
    case GD_RED:
        bulb = bush->getRed();
        break;
    case GD_GREEN:
        bulb = bush->getGreen();
        break;
    case GD_BLUE:
        bulb = bush->getBlue();
        break;
    case GD_WHITE:
        bulb = bush->getWhite();
        break;
    default:
        throw "Invalid Color id";
    }
    bulb->fade(s_intensity,f_intensity,duration);
}

void GregsDisplay::set_bush(int bushId, int color, int intensity)
{
    Bush *bush = model->getBush(bushId);
    if (color == GD_RED)
    {
        bush->getRed()->setIntensity(intensity);
    }
    else if (color == GD_GREEN)
    {
        bush->getGreen()->setIntensity(intensity);
    }
    else if (color == GD_BLUE)
    {
        bush->getBlue()->setIntensity(intensity);
    }
    else if (color == GD_WHITE)
    {
        bush->getWhite()->setIntensity(intensity);
    }
}

