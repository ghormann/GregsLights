#include "../include/Snowmen.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define RIGHT 1
#define LEFT  2
#define ON        100
#define OFF       0



Snowmen::Snowmen()
{
    //ctor
    snowman_t = 0;
    skip_time_check = false;
    strcpy(message2, "Starting up");
}

Snowmen::~Snowmen()
{
    //dtor
}

void Snowmen::run()
{
    time_t now_t;
    struct tm *now;
    while(1)
    {
        time(&now_t);
        now = localtime(&now_t);
        while(now->tm_hour > 8 && now->tm_hour < 17 && !skip_time_check)
        {
            sprintf(message2, "Sleeping 10 minutes(%02d:%02d)",
                    now->tm_hour,
                    now->tm_min);
            sleep(600);
            time(&now_t);
            now = localtime(&now_t);
        }

        do_it_snowmen();
    }
}

void Snowmen::do_it_snowmen()
{
    /*
     * Waited (but random) delay between attemps.
     *
     * 38% 2   second sleep
     * 13% 0.2 second sleep
     * 13% 4   second sleep
     * 13% 6   second sleep
     * 25% 1   second sleep
     */

    switch(random() %8)
    {
    case 0:
    case 1:
    case 2:
        sprintf(message2, "Sleep 2");
        sleep(2);
        break;

    case 3:
        sprintf(message2, "Sleep .2");
        write_data(.2);
        break;

    case 4:
        sprintf(message2, "Sleep 4");
        sleep(4);
        break;

    case 5:
        sprintf(message2, "Sleep 6");
        sleep(6);
        break;

    case 6:
    case 7:
        sprintf(message2, "Sleep 1");
        sleep(1);
        break;
    }


    /*
       * What function is choosen is also random
       *
       * 14% Hit throwing right (.3 second delay)
       * 14% Hit throwing left (.3 second delay)
       * 14% Miss throwing right (.3 second delay)
       * 14% Miss throwiong left (.3 second dleay)
       * 5%  3 misses right + 5 low hits right
       * 9%  1 miss right + 1 hit right
       * 9%  1 miss left + 1 hit left
       * 5%  3 miss left + 5 low hits left.
       * 5%  miss right, miss left, hit right, hit left
       * 5%  miss rifht, 3 miss left, 5 hit low right
       * 5%  Catch going right
       * 5%  Catch going Left
       */

    switch(random() %22)
    {
    case 0:
    case 1:
    case 2:
        hit_high_right(.3);
        break;
    case 3:
    case 4:
    case 5:
        hit_high_left(.3);
        break;

    case 6:
    case 7:
    case 8:
        miss_going_right(.3);
        break;

    case 9:
    case 10:
    case 11:
        miss_going_left(.3);
        break;

    case 12:
        miss_going_right(.3);
        miss_going_right(.3);
        miss_going_right(.3);
        hit_low_right(.2);
        hit_low_right(.2);
        hit_low_right(.1);
        hit_low_right(.1);
        hit_low_right(.1);
        break;

    case 13:
    case 14:
        miss_going_right(.3);
        hit_high_right(.3);
        break;

    case 15:
    case 16:
        miss_going_left(.3);
        hit_high_left(.3);
        break;
    case 17:
        miss_going_left(.3);
        miss_going_left(.3);
        miss_going_left(.3);
        hit_low_left(.2);
        hit_low_left(.2);
        hit_low_left(.1);
        hit_low_left(.1);
        hit_low_left(.1);
        break;

    case 18:
        miss_going_right(.3);
        miss_going_left(.3);
        hit_high_right(.3);
        hit_high_left(.3);
        break;

    case 19:
        miss_going_right(.3);
        miss_going_left(.3);
        miss_going_left(.3);
        miss_going_left(.3);
        hit_low_right(.2);
        hit_low_right(.2);
        hit_low_right(.2);
        hit_low_right(.1);
        hit_low_right(.1);
        break;

    case 20:
        catch_going_right(.3, .2);
        catch_going_right(.3, .2);
        hit_low_left(.2);
        hit_low_left(.2);
        break;

    case 21:
        catch_going_left(.3, .2);
        catch_going_left(.3, .2);
        hit_low_right(.2);
        hit_low_right(.2);
        break;

    }
}

void Snowmen::hit_low_right(double pause)
{

    //strcpy(message2, "Hit Low Right");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(RIGHT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,ON,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    write_data(pause);


    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,ON,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,ON,OFF,OFF,OFF,OFF);
    write_data(pause * 2);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_low_right */


/*
 * Pattern #5
 *
 */

void Snowmen::hit_high_right(double pause)
{

    //strcpy(message2, "Hit HIGH Right");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(RIGHT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,ON,OFF,OFF,OFF,OFF);
    write_data(pause * 2);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_high_right */


/*
 * Pattern #8
 *
 */

void Snowmen::catch_going_right(double pause, double pause2)
{

    //strcpy(message2, "Catch Going Right");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(RIGHT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,ON,OFF,OFF,OFF,OFF,OFF);  /* Catch */
    write_data(pause);

    hit_low_left(pause2);     /* Throw Back */

    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_high_right */




/*
 * Pattern #2
 *
 */

void Snowmen::hit_low_left(double pause)
{
    strcpy(message2, "Hit Low Left");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(LEFT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,ON,OFF);
    write_data(pause);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,ON,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,ON,OFF,OFF,OFF,OFF);
    write_data(pause * 2);


    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_low_left */




/*
 * Pattern #6
 *
 */

void Snowmen::hit_high_left(double pause)
{
    //strcpy(message2, "Hit HIGH Left");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(LEFT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,ON,OFF,OFF,OFF,OFF);
    write_data(pause * 2);


    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_high_left */



/*
 * Pattern #7
 *
 */

void Snowmen::catch_going_left(double pause, double pause2)
{
    //strcpy(message2, "Catch_going_left");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(LEFT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);


    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,ON,OFF,OFF,OFF,OFF,OFF);  /* Catch */
    write_data(pause);

    hit_low_right(pause2); /* Throw back */


    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function Catch Going Left */




/*
 * Pattern #3
 *
 */

void Snowmen::miss_going_right(double pause)
{

    strcpy(message2, "Miss Going Right");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(RIGHT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);

    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,ON);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,ON,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,ON,OFF,OFF,OFF);
    write_data(pause);

    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_low_right */



/*
 * Pattern #4
 *
 */

void Snowmen::miss_going_left(double pause)
{


    //strcpy(message2, "Miss Going Left");
    /*
     * Clear
     */
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

    set_snowmen(LEFT,ON,OFF,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,ON,OFF,OFF,OFF,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,ON,OFF,OFF);
    write_data(pause);
    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,ON);  /* Correct? */
    write_data(pause);

    set_snowmen(LEFT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,ON,OFF);
    write_data(pause);
    set_snowmen(RIGHT,OFF,OFF,OFF,ON,OFF,OFF,OFF);
    write_data(pause * 2);


    set_snowmen(RIGHT,OFF,OFF,OFF,OFF,OFF,OFF,OFF);

} /* End function hit_low_left */


/*
  Just run some test statuses
*/
void Snowmen::test_snowmen(void)
{
    int i;

    for (i=1; i < 3; i++)
    {
        sprintf(message2, "i = %d  Step 1\n", i);
        set_snowmen(i,ON,OFF,OFF,OFF,OFF,OFF,OFF);
        //printf ("%s\n", message2);
        write_data(3);
        sprintf(message2, "i = %d  Step 2\n", i);
        set_snowmen(i,OFF,ON,OFF,OFF,OFF,OFF,OFF);
        //printf ("%s\n", message2);
        write_data(3);
        sprintf(message2, "i = %d  Step 3\n", i);
        set_snowmen(i,OFF,OFF,ON,OFF,OFF,OFF,OFF);
        //printf ("%s\n", message2);
        write_data(3);
        sprintf(message2, "i = %d  Step 4\n", i);
        set_snowmen(i,OFF,OFF,OFF,ON,OFF,OFF,OFF);
        //printf ("%s\n", message2);
        write_data(3);
        sprintf(message2, "i = %d  Step 5\n", i);
        set_snowmen(i,OFF,OFF,OFF,OFF,ON,OFF,OFF);
        //printf ("%s\n", message2);
        write_data(3);
        sprintf(message2, "i = %d  Step 6\n", i);
        set_snowmen(i,OFF,OFF,OFF,OFF,OFF,ON,OFF);
        //printf ("%s\n", message2);
        write_data(3);
        sprintf(message2, "i = %d  Step 7\n", i);
        set_snowmen(i,OFF,OFF,OFF,OFF,OFF,OFF,ON);
        //printf ("%s\n", message2);
        write_data(3);
        // Clear current snowman
        set_snowmen(i,OFF,OFF,OFF,OFF,OFF,OFF,OFF);
    }
} /* end test_snowmen */

char * Snowmen::getMessage() {
    return message2;
}

void Snowmen::set_snowmen(int side, int a, int b,int c,int d,int e,int f,int g)
{
    int offset = ((side == RIGHT) ? 7 : 0);

    bulbs[0+offset]->setIntensity(a);
    bulbs[1+offset]->setIntensity(b);
    bulbs[2+offset]->setIntensity(c);
    bulbs[3+offset]->setIntensity(d);
    bulbs[4+offset]->setIntensity(e);
    bulbs[5+offset]->setIntensity(f);
    bulbs[6+offset]->setIntensity(g);
}

void Snowmen::setBulb(int i, Bulb *b)
{
    if (i < 0 ||  i>= SNOWMEN_MAX_BULBS)
    {
        throw "Snowmen::setBulb - bulb was out of range";
    }

    bulbs[i] = b;
}

