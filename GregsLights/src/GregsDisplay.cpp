#include "../include/GregsDisplay.h"
#include "../include/controller/Bush.h"
#include <string.h>
#include <iostream>

#define RED 0
#define GREEN 1
#define BLUE 2
#define WHITE 3

using namespace std;

GregsDisplay::GregsDisplay(DisplayModel *m)
{
    this->model = m;
    timeinfo = new TimeInfo(m->isSkipTimeCheck(), m->isNewYears());
}

GregsDisplay::~GregsDisplay()
{
    //dtor
}

void GregsDisplay::doGrinch()
{
    double duration = 10.0;
    strcpy(model->getMessage(1),"Grinch\n");
    model->getGrid()->setNextAction(LG_GRINCH);
    write_data(1.0); // Give it a second to load.
    model->getStars()->fadeAllTo(0,100,0,duration);
    fadeAllBush(GREEN,0,100,duration);
    fadeAllHouse(GREEN,0,100,duration);
    write_data(18.0);
    model->getStars()->fadeAllTo(0,0,0,duration);
    fadeAllBush(GREEN,100,0,duration);
    fadeAllHouse(GREEN,100,0,duration);
    write_data(duration);

}

void GregsDisplay::doClark()
{
    model->getGrid()->setNextAction(LG_CLARK);
    strcpy(model->getMessage(1),"Clark Plug in\n");
    write_data(5);
    setAllToColor(WHITE,100);
    setAllHouse(WHITE,100);
    model->getStars()->setAll(100,100,100);
    write_data(32-5);
    setAllOff();
}

void GregsDisplay::peekHat()
{
    double duration = 8.0;
    model->getGrid()->setNextAction(LG_HAT);
    strcpy(model->getMessage(1),"Peak hat\n");
    fadeAllBush(WHITE,0,100,duration);
    fadeAllHouse(RED,0,100,duration);
    model->getStars()->fadeAllTo(100,100,100,duration);
    write_data(duration);
    fadeAllBush(WHITE,100,0,duration);
    fadeAllHouse(RED,100,0,duration);
    model->getStars()->fadeAllTo(0,0,0,duration);
    write_data(duration);

}

void GregsDisplay::hormannTrain()
{
    double duration = 5.0;
    model->getGrid()->setNextAction(LG_TRAIN_HORMANN);
    fadeAllHouse(RED,0,100,duration);
    fadeAllBush(BLUE,0,100,duration);
    write_data(20);

}


void GregsDisplay::do_it_bushes()
{
    sleep(2); // GIve everything time to Boot up.
    while(1)
    {
        setAllOff();
        sleep(1);

        while (! timeinfo->isDisplayHours())
        {
            sprintf(model->getMessage(1), "Sleeping during day (%02d)",
                    timeinfo->getHourOfDay());
            setAllOff();
            sleep(5);
        }
        /*
                // STEP: CLARK
                setAllOff();
                write_data(0.1);
                doClark();


                //STEP Horman Train
                hormannTrain();

                // STEP: Grinch
                setAllOff();
                write_data(0.1);
                doGrinch();


                // STEP: Rotate_some
                model->getGrid()->setNextAction(LG_SHOW_PICT);
                write_data(0.5); // Give grid a second
                setAllOff();
                write_data(0.1);
                rotate_some();

                // STEP: HAt
                peekHat();

                // STEP 2
                model->getGrid()->setNextAction(LG_SHOW_PICT);
                fadeThroughAll(5, 2);

                // STEP 3
                //cout << "Step 3" << endl;
                model->getGrid()->setNextAction(LG_FIRE);
                fade_offset();


                // Step 4
                model->getGrid()->setNextAction(LG_SHOW_PICT);
                moveFasterRight();


        // Step 5
        model->getGrid()->setNextAction(LG_SHOW_PICT);
        moveFromMiddle();
        // Step 6
        fadeWhite();
        */
        // Step 7
        redGreenFade();
        // Step 8
        change_one();  // Still doesn't fade 100% correctly.

    }

}

void GregsDisplay::run()
{
    pthread_t sign_t;
    pthread_t bush_t;
    pthread_t snowmen_t;
    pthread_t grid_t;
    pthread_create(&(sign_t), NULL, GregsDisplay::signThread, (void*) this);
    pthread_create(&(bush_t), NULL, GregsDisplay::bushThread, (void*) this);
    pthread_create(&(snowmen_t), NULL, GregsDisplay::snowmenThread, (void*) this);
    pthread_create(&(grid_t), NULL, GregsDisplay::gridThread, (void*) this);

    getModel()->getClock()->setActive(true);
}

void GregsDisplay::fadeAllBush(int color, int s_intensity, int f_intensity, double duration)
{
    int i;
    for (i = 1; i <= 6; i++)
    {
        fade_bush(i, color, s_intensity, f_intensity, duration);
    }
}
void GregsDisplay::fadeAllHouse(int color, int s_intensity, int f_intensity, double duration)
{
    int i;
    for (i = 1; i <= 4; i++)
    {
        fade_house(i, color, s_intensity, f_intensity, duration);
    }
}
void GregsDisplay::setAllHouse(int color, int intensity)
{
    int i;
    for (i = 1; i <= 4; i++)
    {
        set_house(i, color, intensity);
    }
}



/*
 * Set All bushes to one color
 */
void GregsDisplay::setAllToColor(int color, int intensity)
{
    int i;
    for (i = 1; i <= 6; i++)
    {
        set_bush(i, color, intensity);
    }
}

/*
 * Set all bushes and house lights off
 */

void GregsDisplay::redGreenFade()
{
    int i,j,r,g,b;
    int left = 0%2;
    int right = 1%2;
    double duration = 1.0;

    setRGB(left,r,g,b);
    model->getStars()->fadeLeft(r,g,b,duration);
    setRGB(right,r,g,b);
    model->getStars()->fadeRight(r,g,b,duration);


    for(i = 1; i<=6; i++)
    {
        fade_bush(i, i<=3? left : right ,0,100,duration);
        if (i <= 4)
            fade_house(i, i<=2? left : right ,0,100,duration);
    }
    write_data(duration);

    j = 0;
    duration = 4.0;
    while (duration > 1)
    {
        j++;
        left = j%2;
        right = (j+1)%2;
        sprintf(model->getMessage(1),"RedGreenFade: %f", duration);
        setRGB(left,r,g,b);
        model->getStars()->fadeLeft(r,g,b,duration);
        setRGB(right,r,g,b);
        model->getStars()->fadeRight(r,g,b,duration);
        for(i = 1; i<=6; i++)
        {
            fade_bush(i, i<=3? left : right ,0,100,duration);
            fade_bush(i, i<=3? right : left ,100,0,duration);
            if (i <= 4)
            {
                fade_house(i, i<=2? left : right ,0,100,duration);
                fade_house(i, i<=2? right : left ,100, 0,duration);
            }
        }
        write_data(duration);
        duration -= (duration*.15);
    }

    j++;
    left = j%2;
    right = (j+1)%2;
    model->getStars()->fadeAllTo(100,100,0,duration);
    for (i = 1; i<=6; i++)
    {
        fade_bush(i, i<=3? left : right ,0,100,1.0);
        if (i <= 4)
            fade_house(i, i<=2? left : right ,0,100,duration);
    }
    write_data(4.0);

}

/*
 * Fade the bushes from one color to another, but offset
 * the busshes to run at different times.
 */
void GregsDisplay::fade_offset()
{
    int i;
    int color;
    double duration=6;
    double sleepBetween = duration/6;

    sprintf(model->getMessage(1),"Fade Offset");
    fadeAllBush(WHITE,0,100,3.0);
    fadeAllHouse(BLUE,0,100,3.0);
    model->getStars()->fadeAllTo(100,100,100,3.0);
    //setAllToColor(WHITE,100);
    //setAllHouse(BLUE,100);

    write_data(3.0);

    for (color=RED; color <=WHITE; color++)
    {
        for(i=1; i<=6; i++)
        {
            fade_bush(i,color,0,100,duration);
            fade_bush(i,(color+3)%4,100,0,duration);
            int r = (color == RED || color == WHITE ? 100 : 0);
            int g = (color == GREEN || color == WHITE ? 100 : 0);
            int b = (color == BLUE || color == WHITE ? 100 : 0);
            model->getStars()->fadeLine(i,r,g,b,duration);
            if (i == 1)
                model->getStars()->fadeLine(0,r,g,b,duration);
            if (i== 6)
                model->getStars()->fadeLine(7,r,g,b,duration);

            write_data(sleepBetween);
        }
    }
    write_data(duration-sleepBetween);
    sprintf(model->getMessage(1),"Fade Offset - DOne");
    setAllToColor(WHITE,100);
    setAllToColor(RED,0);
    setAllToColor(GREEN,0);
    setAllToColor(BLUE,0);
    write_data(0.1);
    for(i=1; i<=6; i++)
    {
        if (i <= 4)
            fade_house(i,BLUE,100,0,5.0);
        fade_bush(i,WHITE,100,0,5.0);
    }
    model->getStars()->fadeAllTo(0,0,0,5.0);
    write_data(5.0);
    setAllHouse(BLUE,0);
    setAllOff();
    write_data(0.1);
}


/*
 * Rotate All Bushes
 */
void GregsDisplay::rotate_all(double duration, int cycles)
{
    int i;
    int loop=0;
    // Rotate all bushes
    while(loop < cycles)
    {
        loop++;
        sprintf(model->getMessage(1),"Rotate the Bushes with set: %i or %i", loop, cycles);
        for(i=RED; i<=WHITE; i++)
        {
            setAllToColor(i, 100);
            if (i == RED || i == GREEN)
            {
                setAllHouse(i,100);
            }
            write_data(duration);
            setAllToColor(i, 0);
            if (i == RED || i == GREEN)
            {
                setAllHouse(i,0);
            }
        }
    }
}



/*
 * Fade RED->GREEN->BLUE->->RED/GREEN/->WHITE->BLUE/GREEN
 * f_duration-> Fade Duration
 * h_duration-> Hold duration (after fade done.)
 */
void GregsDisplay::fadeThroughAll(double f_duration, double h_duration)
{
    int i;

    strcpy(model->getMessage(1),"FADE Through: Fade All RED\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, RED, 0, 100, f_duration);
        if (i <= 4)
            fade_house(i, RED, 0, 100, f_duration);
    }
    model->getStars()->fadeAllTo(100,0,0, f_duration);
    write_data(f_duration);
    for (i=1; i<=4; i++)
        set_house(i, RED, 100);

    setAllToColor(RED, 100);
    setAllToColor(GREEN, 0);
    setAllToColor(BLUE, 0);
    setAllToColor(WHITE, 0);
    write_data(h_duration);

    strcpy(model->getMessage(1),"FADE THROUGH: Fade All RED->GREEN\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, RED, 100, 0, f_duration);
        fade_bush(i, GREEN, 0, 100, f_duration);
        if (i <= 4)
        {
            fade_house(i, RED, 100, 0, f_duration);
            fade_house(i, GREEN, 0, 100, f_duration);
        }
    }
    model->getStars()->fadeAllTo(0,100,0, f_duration);
    write_data(f_duration);
    for (i=1; i<=4; i++)
    {
        set_house(i, RED, 0);
        set_house(i, GREEN, 100);
    }
    setAllToColor(GREEN, 100);
    setAllToColor(RED, 0);
    write_data(h_duration);

    strcpy(model->getMessage(1),"FADE THROUGH: Fade All GREEN->BLUE\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, GREEN, 100, 0, f_duration);
        fade_bush(i, BLUE, 0, 100, f_duration);
        if (i <= 4)
        {
            fade_house(i, GREEN, 100, 0, f_duration);
            fade_house(i, BLUE, 0, 100, f_duration);
        }
    }

    model->getStars()->fadeAllTo(0,0,100, f_duration);
    write_data(f_duration);
    for (i=1; i<=4; i++)
    {
        set_house(i, GREEN, 0);
        set_house(i, BLUE, 100);
    }
    setAllToColor(BLUE, 100);
    setAllToColor(GREEN,0);
    write_data(h_duration);

    strcpy(model->getMessage(1),"FADE THROUGH: Fade All BLUE->RED+GREEN\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, BLUE, 100, 0, f_duration);
        fade_bush(i, RED, 0, 100, f_duration);
        fade_bush(i, GREEN, 0, 100, f_duration);
        if (i <= 4)
        {
            fade_house(i, RED,  0, 100, f_duration);
            fade_house(i, BLUE,  100, 0, f_duration);
            fade_house(i, GREEN, 0, 100, f_duration);
        }
    }

    model->getStars()->fadeAllTo(100,100,0, f_duration);
    write_data(f_duration);
    for (i=1; i<=4; i++)
    {
        set_house(i, RED, 100);
        set_house(i, GREEN, 100);
        set_house(i, BLUE, 0);
    }
    setAllToColor(RED, 100);
    setAllToColor(GREEN, 100);
    setAllToColor(BLUE, 0);
    write_data(h_duration);


    strcpy(model->getMessage(1),"FADE THROUGH: Fade All RED+GREEN->WHITE\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, RED, 100, 0, f_duration);
        fade_bush(i, GREEN, 100, 0, f_duration);
        fade_bush(i, WHITE, 0, 100, f_duration);

        if (i <= 4)
        {
            fade_house(i, RED,  100, 0, f_duration);
            fade_house(i, GREEN, 100, 0, f_duration);
        }
    }

    model->getStars()->fadeAllTo(100,100,100,f_duration);
    write_data(f_duration);
    for (i=1; i<=4; i++)
    {
        set_house(i, RED, 0);
        set_house(i, GREEN, 0);
    }
    setAllToColor(WHITE, 100);
    setAllToColor(RED, 0);
    setAllToColor(GREEN, 0);
    write_data(h_duration);

    strcpy(model->getMessage(1),"FADE THROUGH: WHITE->BLUE/GREEN\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, WHITE, 100, 0, f_duration);
        fade_bush(i, GREEN, 0, 100, f_duration);
        fade_bush(i, BLUE, 0, 100, f_duration);
        if (i <= 4)
        {
            if (i%2)
                fade_house(i, BLUE,  0, 100, f_duration);
            else
                fade_house(i, GREEN, 0, 100, f_duration);
        }
    }
    model->getStars()->fadeAllTo(0,100,100,f_duration);
    write_data(f_duration);
    setAllToColor(WHITE, 0);
    setAllToColor(BLUE, 100);
    setAllToColor(GREEN, 100);
    setAllToColor(RED, 0);
    for (i=1; i<=4; i++)
    {
        if (i%2)
            set_house(i, BLUE, 100);
        else
            set_house(i, GREEN, 100);
    }
    write_data(h_duration);

    strcpy(model->getMessage(1),"FADE THROUGH: BLUE/GREEN->OFF\n");
    for (i=1; i<=6; i++)
    {
        fade_bush(i, GREEN, 100, 0, f_duration);
        fade_bush(i, BLUE, 100, 0, f_duration);
        if (i <= 4)
        {
            if (i%2)
                fade_house(i, BLUE,  100, 0, f_duration);
            else
                fade_house(i, GREEN, 100, 0, f_duration);
        }
    }
    model->getStars()->fadeAllTo(0,0,0, f_duration);
    write_data(f_duration);
    setAllToColor(WHITE, 0);
    setAllToColor(BLUE, 0);
    setAllToColor(GREEN, 0);
    setAllToColor(RED, 0);
    setAllHouse(BLUE,0);
    setAllHouse(GREEN,0);

}

void GregsDisplay::chase_left(int baseColor, int diffColor, double startDuration,
                              double decreasePct, double stopDuration)
{
    double duration = startDuration;
    int bush = 1;
    int i;
    sprintf(model->getMessage(1),"Chase LEFT - Duration: %f", duration);
    //setAllOff();
    for (i=RED; i <=WHITE; i++)
    {
        if (i == baseColor)
            setAllToColor(i, 1000);
        else
            setAllToColor(i, 0);
    }
    write_data(.01);

    setAllToColor(baseColor, 100);
    setAllHouse(baseColor, 100);
    write_data(.1);
    setAllToColor(baseColor, 100);
    setAllHouse(baseColor, 100);
    write_data(.1);

    while (duration > stopDuration)
    {
        sprintf(model->getMessage(1),"Chase LEFT - Duration: %f", duration);
        set_bush(bush, baseColor, 0);
        set_bush(bush, diffColor, 100);
        write_data(duration);
        set_bush(bush, baseColor, 100);
        set_bush(bush, diffColor, 0);

        if (--bush < 1)
            bush = 6;

        duration -= (duration * decreasePct);
    }

}


void GregsDisplay::chase_right(int baseColor, int diffColor, double startDuration,
                               double decreasePct, double stopDuration)
{
    double duration = startDuration;
    int r,g,b;
    int bush = 1;
    sprintf(model->getMessage(1),"Chase Right - Duration: %f", duration);
    setAllOff();
    setRGB(baseColor,r,g,b);
    model->getStars()->setAll(r,g,b);
    setAllToColor(baseColor, 100);
    setAllHouse(baseColor, 100);
    write_data(.1);
    setAllToColor(baseColor, 100);
    setAllHouse(baseColor, 100);
    write_data(.1);

    while (duration > stopDuration)
    {
        setRGB(diffColor,r,g,b);
        sprintf(model->getMessage(1),"Chase Right - Duration: %f", duration);
        set_bush(bush, baseColor, 0);
        set_bush(bush, diffColor, 100);
        model->getStars()->setLine(bush,r,g,b);
        if (bush == 1)
            model->getStars()->setLine(0,r,g,b);
        if (bush == 6)
            model->getStars()->setLine(7,r,g,b);
        write_data(duration);
        set_bush(bush, baseColor, 100);
        set_bush(bush, diffColor, 0);
        setRGB(baseColor,r,g,b);
        model->getStars()->setLine(bush,r,g,b);
        if (bush == 1)
            model->getStars()->setLine(0,r,g,b);
        if (bush == 6)
            model->getStars()->setLine(7,r,g,b);

        if (++bush > 6)
            bush = 1;

        duration -= (duration * decreasePct);
    }

}

void GregsDisplay:: setRGB(int color, int &r, int &g, int &b)
{
    r=g=b=0;
    if (color == RED || color == WHITE)
        r = 100;
    if (color == GREEN || color == WHITE)
        g = 100;
    if (color == BLUE || color == WHITE)
        b = 100;
}

/*
 * Change_one
 *
 * Set each bush to a color and then change them
 * one at a time.
 */

void GregsDisplay::change_one()
{
    int i, j;
    int colors[10];
    double duration = 3.0;
    int mult = 3;
    sprintf(model->getMessage(1),"Change_one: Set initial Colors");
    setAllOff();

    for (i = 1; i <=6; i++)
    {
        colors[i] = i;
    }
    colors[7] = 0; // House



    /*
    || Colors
    || 1 = RED
    || 2 = GREEN
    || 3 = BLUE
    || 4 = RED + Green
    || 5 = WHITE
    || 6 = Green + Blue
    */
    for (i = 9; i >= 0; i--)
    {
        sprintf(model->getMessage(1),"Change_one: Position %d", i);
        for (j = 1; j <=6; j++)
        {
            sprintf(model->getMessage(1),"Change_one: Position %d, %d", i, colors[j]);
            switch (colors[j])
            {
            case 1:
                fade_bush(j, RED, 0, 100, duration);
                fade_bush(j, BLUE, 100, 0, duration);
                fade_bush(j, GREEN, 100, 0, duration);
                break;
            case 2:
                fade_bush(j, GREEN, 0, 100, duration);
                fade_bush(j, RED, 100, 0, duration);
                break;
            case 3:
                fade_bush(j, BLUE, 0, 100, duration);
                fade_bush(j, GREEN, 100, 0, duration);
                break;
            case 4:
                fade_bush(j, RED, 0, 100, duration);
                fade_bush(j, GREEN, 0, 100, duration);
                fade_bush(j, BLUE, 100, 0, duration);
                break;
            case 5:
                fade_bush(j, WHITE, 0, 100, duration);
                fade_bush(j, RED, 100, 0, duration);
                fade_bush(j, GREEN, 100, 0, duration);
                break;
            default:
                fade_bush(j, BLUE, 0, 100, duration);
                fade_bush(j, GREEN, 0, 100, duration);
                fade_bush(j, WHITE, 100, 0, duration);
                break;
            }

            ++colors[j];
            if (colors[j] > 6)
            {
                colors[j] = 1;
            }
        }
        if ( (i%mult) == 0)
        {
            ++colors[7];
            if (colors[7] > 3)
            {
                colors[7] = 0;
            }
            switch (colors[7])
            {
            case 0:
                fadeAllHouse(RED, 0, 100, duration*mult);
                fadeAllHouse(BLUE, 100, 0, duration*mult);
                break;
            case 1:
                fadeAllHouse(BLUE, 0, 100, duration*mult);
                fadeAllHouse(RED, 100, 0, duration*mult);
                break;
            case 2:
                fadeAllHouse(RED, 0, 100, duration*mult);
                fadeAllHouse(GREEN, 0, 100, duration*mult);
                fadeAllHouse(BLUE, 100, 0, duration*mult);
                break;
            case 3:
                fadeAllHouse(BLUE, 0, 100, duration*mult);
                fadeAllHouse(RED, 100, 0, duration*mult);
                fadeAllHouse(GREEN, 100, 0, duration*mult);
                break;
            }
        }
        write_data(duration * 1.5);
    }

    for (int i = HOUSE_LIGHT_START; i <= HOUSE_LIGHT_END; i++)
    {
        model->getHouse(i)->fadeTo(0,0,0, 3.0);
    }

    for (int i = BUSH_LIGHT_START; i <= BUSH_LIGHT_END; i++)
    {
        model->getBush(i)->getBlue()->fadeTo(0,3.0);
        model->getBush(i)->getRed()->fadeTo(0,3.0);
        model->getBush(i)->getGreen()->fadeTo(0,3.0);
        model->getBush(i)->getWhite()->fadeTo(0,3.0);
    }
    write_data(3.0);

}

void GregsDisplay::fadeWhite()
{
    double duration = 5.0;
    int i, j;
    sprintf(model->getMessage(1),"Fade WHITE from 100->35 down and back");
    for (i=1; i <= 4; i++)
    {
        if (i%2)
            fade_house(i, GREEN, 0, 100, 2.0);
        else
            fade_house(i, RED, 0, 100, 2.0);
    }
    fadeAllBush(WHITE, 0, 100, 2.0);
    model->getStars()->fadeAllTo(100,100,100,2.0);
    write_data(2.0);

    for (i=1; i < 3; i++)
    {
        for (j=1; j <= 4; j++)
        {
            if (j%2)
                fade_house(j, GREEN, 100, 35, duration);
            else
                fade_house(j, RED, 100, 35, duration);
        }
        fadeAllBush(WHITE, 100, 35, duration);
        model->getStars()->fadeAllTo(10,10,10,duration);
        write_data(duration);

        for (j=1; j <= 4; j++)
        {
            if (j%2)
                fade_house(j, GREEN, 35, 100, duration);
            else
                fade_house(j, RED, 35, 100, duration);
        }
        fadeAllBush(WHITE, 35, 100, duration);
        model->getStars()->fadeAllTo(100,100,100,duration);
        write_data(duration);
    }
    sprintf(model->getMessage(1),"Fade WHITE OFF");
    model->getStars()->fadeAllTo(0,0,0,duration);
    for (i=1; i<=6; i++)
    {
        fade_bush(i, WHITE, 100, 0, duration);
    }
    for (i=1; i <= 4; i++)
    {
        if (i%2)
            fade_house(i, GREEN, 100, 0, duration);
        else
            fade_house(i, RED, 100, 0, duration);
    }
    write_data(duration);
    setAllOff();
    setAllHouse(GREEN, 0);
    setAllHouse(RED, 0);
}

void GregsDisplay::moveFasterRight()
{
    double duration = 3.0;
    int i;
    strcpy(model->getMessage(1),"MoveFasterRight");
    setAllOff();
    fadeAllBush(RED,0,100,3.0);
    fadeAllHouse(RED,0,100,3.0);
    model->getStars()->fadeAllTo(100,0,0,3.0);

    write_data(3.0);

    chase_right(RED, GREEN, 1.3, 0.05, 0.1);
    setAllToColor(RED, 100);
    setAllToColor(GREEN, 0);
    write_data(.1);
    for (i=1; i<=6; i++)
    {
        fade_bush(i, WHITE, 0, 100, duration);
        fade_bush(i, RED, 100, 0, duration);
        if (i <= 4)
        {
            fade_house(i, BLUE, 0, 100, duration);
            fade_house(i, RED, 100, 0, duration);
        }
    }
    write_data(duration + 0.1);
    setAllToColor(RED, 0);
    setAllHouse(RED,0);
}
void GregsDisplay::moveFasterLeft()
{
    int i=0;
    double duration=2.0;

    strcpy(model->getMessage(1),"MoveFasterLeft");

    chase_left(GREEN, RED, 1.3, 0.05, 0.1);
    setAllToColor(GREEN, 100);
    setAllToColor(RED, 0);
    write_data(.1);
    for (i=1; i<=6; i++)
    {
        fade_bush(i, GREEN, 100, 0, duration);
        if (i <= 4)
        {
            fade_house(i, GREEN, 100, 0, duration);
        }
    }
    write_data(duration + 0.1);
    setAllToColor(RED, 0);
    setAllHouse(RED, 0);
    setAllToColor(GREEN, 0);
    setAllHouse(GREEN, 0);
}

void GregsDisplay::moveFromMiddle()
{
    int i=0, j=0;
    int bush1 = 3;
    int bush2 = 4;
    int iGreen = 0;
    int iRed = 0;
    int iWhite = 0;
    int r,g,b;
    double duration=2.0;

    strcpy(model->getMessage(1),"MoveFromMiddle");

    setAllHouse(BLUE,100);
    write_data(duration + 0.1);
    setAllToColor(GREEN, 100);

    for (i = 0; i < 8; i++)
    {
        setRGB(i%(WHITE+1),r,g,b);
        model->getStars()->fadeLine(i,r,g,b,10.0);
    }

    // Really Start it
    duration=1.3;
    for (i=0; i < 50; i++)
    {
        sprintf(model->getMessage(1),"MoveFromMiddle - %f, %d ", duration, i);
        cout << "Pos: " << i << endl;
        if (i == 15)
        {
            for (int k = 0; k < 8; k++)
            {
                setRGB((k+1)%(WHITE+1),r,g,b);
                if (r == 0 )
                    r = 100;
                model->getStars()->fadeLine(k,r,g,b,10.0);
            }
        }

        for(j=1; j<=6; j++)
        {
            if (j == bush1 || j == bush2 )
            {
                iGreen = 100;
                iRed   = 100;
                iWhite = 0;
            }
            else
            {
                iGreen = 0;
                iRed   = 0;
                iWhite = 100;
            }
            set_bush(j,GREEN,iGreen);
            set_bush(j,RED,iRed);
            set_bush(j,WHITE,iWhite);
        }
        write_data(duration);
        --bush1;
        ++bush2;
        if (bush1 < 0 )
        {
            bush1 = 3;
            bush2 = 4;
            duration = duration *.8;
        }

    }
    write_data(duration);

    // DONE
    setAllToColor(RED, 0);
    setAllToColor(GREEN, 0);
    setAllToColor(WHITE, 100);
    write_data(.1);
    fadeAllBush(RED, 0, 100, 10.0);
    fadeAllBush(GREEN, 0, 100, 10.0);
    fadeAllBush(WHITE, 100, 0, 10.0);
    write_data(10.0);
    setAllToColor(RED, 100);
    setAllToColor(GREEN, 100);
    write_data(2.0);

    setAllOff();
    write_data(0);
}



void GregsDisplay::rotate_some()
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
            for (color=RED; color<=WHITE; color++)
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

    duration = 4.0;
    for (i = 1; i<=6; i++)
    {
        fade_bush(i, (i-1)%4, 100,0, duration);
    }

    for (int i = 1; i <= 4; i++)
    {
        model->getHouse(i)->fadeTo(0,0,0,duration);
    }

    write_data(duration);

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

    model->getStars()->setAll(RGBColor::BLACK);

}

void GregsDisplay::fade_bush(int bushId, int color, int s_intensity, int f_intensity, double duration)
{
    if (bushId < BUSH_LIGHT_START || bushId > BUSH_LIGHT_END)
    {
        sprintf(getModel()->getMessage(1), "Invalid Bush ID: %d", bushId);
        sleep(3);
        throw "Illegal Bush ID";
    }
    Bush *bush = model->getBush(bushId);
    Bulb *bulb;
    switch(color)
    {
    case RED:
        bulb = bush->getRed();
        break;
    case GREEN:
        bulb = bush->getGreen();
        break;
    case BLUE:
        bulb = bush->getBlue();
        break;
    case WHITE:
        bulb = bush->getWhite();
        break;
    default:
        sprintf(getModel()->getMessage(1), "Invalid Color ID: %d", color);
        sleep(1);
        throw "Invalid Color id";
    }
    bulb->fade(s_intensity,f_intensity,duration);
}

void GregsDisplay::fade_house(int lightId,int color, int s_intensity,int f_intensity,double duration)
{
    RGBLight *light = model->getHouse(lightId);
    if (color == RED)
    {
        light->getRedBulb()->fade(s_intensity,f_intensity,duration);
    }
    else if (color == GREEN)
    {
        light->getGreenBulb()->fade(s_intensity,f_intensity,duration);
    }
    else if (color == BLUE)
    {
        light->getBlueBulb()->fade(s_intensity,f_intensity,duration);
    }

}

void GregsDisplay::set_house(int lightId,int color,int intensity)
{
    RGBLight *light = model->getHouse(lightId);
    if (color == RED)
    {
        light->getRedBulb()->setIntensity(intensity);
    }
    else if (color == GREEN)
    {
        light->getGreenBulb()->setIntensity(intensity);
    }
    else if (color == BLUE)
    {
        light->getBlueBulb()->setIntensity(intensity);
    }
    else if (color == WHITE)
    {
        light->getBlueBulb()->setIntensity(intensity);
        light->getGreenBulb()->setIntensity(intensity);
        light->getRedBulb()->setIntensity(intensity);
    }

}

void GregsDisplay::set_bush(int bushId, int color, int intensity)
{
    Bush *bush = model->getBush(bushId);
    if (color == RED)
    {
        bush->getRed()->setIntensity(intensity);
    }
    else if (color == GREEN)
    {
        bush->getGreen()->setIntensity(intensity);
    }
    else if (color == BLUE)
    {
        bush->getBlue()->setIntensity(intensity);
    }
    else if (color == WHITE)
    {
        bush->getWhite()->setIntensity(intensity);
    }
}

DisplayModel *GregsDisplay::getModel()
{
    return model;
}

void * GregsDisplay::gridThread(void *args)
{
    GregsDisplay *ptr = (GregsDisplay *) args;
    while(1)
    {
        ptr->getModel()->getGrid()->run();
    }
    return NULL;

}

void * GregsDisplay::snowmenThread(void * args)
{
    GregsDisplay *ptr = (GregsDisplay *) args;
    while(1)
    {
        ptr->getModel()->getSnowmen()->run();
    }
    return NULL;
}


void * GregsDisplay::signThread(void *args)
{
    GregsDisplay *ptr = (GregsDisplay *) args;
    while (1)
    {
        ptr->getModel()->getSign()->run();
    }
    return NULL;
}


void *GregsDisplay::bushThread(void *args)
{
    GregsDisplay *ptr = (GregsDisplay *) args;
    while(1)
    {
        ptr->do_it_bushes();
    }
    return NULL;
}

