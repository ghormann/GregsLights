#include "../include/Snowmen.h"
#include "../include/controller/DummyBulb.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "RGBPicture.h"


#define SNOWBALL_DURATION 0.015
#define BALL_SIZE_1IN   3
#define BALL_SIZE_2IN   1.7
#define CANNON_BALL_SIZE_1IN 11.9
#define CANNON_BALL_SIZE_2IN 5.9


Snowmen::Snowmen(bool skipTime,  E131Network *network[], GregMQTT *mqtt)
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        throw "Mutext init failed for Snowmen";
    }
    this->mqtt = mqtt;
    groundSnowLevel[SNOWMAN_LEFT] = groundSnowLevel[SNOWMAN_RIGHT]= 0;
    noseBalls[SNOWMAN_LEFT] = noseBalls[SNOWMAN_RIGHT] = 0;
    hatStatus[SNOWMAN_LEFT] = true;
    hatStatus[SNOWMAN_RIGHT] = true;
    noseBallColor = RGBColor::WHITE;
    who_right = Snowman;

    sprintf(message_who, "SNOWMEN");

    //ctor
    timeinfo = TimeInfo::getInstance();

    snowmen[0] = new SnowmenGrid(SNOWMEN_WIDTH,SNOWMEN_HEIGHT,SNOWMEN_WIDTH, SNOWMEN_HEIGHT);
    snowmen[1] = new SnowmenGrid(SNOWMEN_WIDTH,SNOWMEN_HEIGHT,SNOWMEN_WIDTH, SNOWMEN_HEIGHT);
    splash[0] = new SnowmenGrid(SPLASH_GRID_WIDTH,SPLASH_GRID_HEIGHT,SPLASH_GRID_WIDTH, SPLASH_GRID_HEIGHT);
    splash[1] = new SnowmenGrid(SPLASH_GRID_WIDTH,SPLASH_GRID_HEIGHT,SPLASH_GRID_WIDTH, SPLASH_GRID_HEIGHT);
    skyGrid = new SnowmenGrid(SKY_GRID_WIDTH,SKY_GRID_HEIGHT,SKY_GRID_WIDTH, SKY_GRID_HEIGHT);

    int x = 0;
    int y = 0;
    int y_start = 0;
    int y_delta = -1; // Will be switched at start -1 = down, 1 = up
    int y_cnt = 0;
    int port = 0; // Array is zero based.
    int channel = 0; // also zero based.
    int pixels_in_string = 0;

    //Snowman house aka Right
    for (x = SNOWMEN_WIDTH-1; x >= 0; x--)
    {
        y_delta *= -1; // Swap direction
        y_start = (y_delta == 1 ? 0 : SNOWMEN_HEIGHT - 1);
        y_cnt = 0;

        for (y = y_start; y_cnt < SNOWMEN_HEIGHT; y += y_delta, ++y_cnt )
        {
            snowmen[SNOWMAN_RIGHT]->setPixal(x,SNOWMEN_HEIGHT-1 - y, network[port]->getRGB(channel));
            channel += 3;
            pixels_in_string++;
            if (pixels_in_string >= 1000)
            {
                pixels_in_string = 0;
                channel = 0;
                ++port;
            }
            if (channel >= 510)
            {
                channel = 0;
                ++port;
            }
        }
    }
    printf("DEBUG: Snowmen Right Port: %d, channel: %d\n", port, channel);

    //Splash house aka Right
    y_delta = 1; // Will be switched at start -1 = down, 1 = up

    for (x = SPLASH_GRID_WIDTH-1; x >= 0; x--)
    {
        y_delta *= -1; // Swap direction
        y_start = (y_delta == 1 ? 0 : SPLASH_GRID_HEIGHT - 1);
        y_cnt = 0;

        for (y = y_start; y_cnt < SPLASH_GRID_HEIGHT; y += y_delta, ++y_cnt )
        {
            splash[SNOWMAN_RIGHT]->setPixal(x,SPLASH_GRID_HEIGHT-1 - y, network[port]->getRGB(channel));
            channel += 3;
            pixels_in_string++;
            if (pixels_in_string >= 1000)
            {
                pixels_in_string = 0;
                channel = 0;
                ++port;
            }
            if (channel >= 510)
            {
                channel = 0;
                ++port;
            }
        }
    }
    printf("DEBUG: SPLASH Right Port: %d, channel: %d\n", port, channel);

    //SKY house aka Right
    y_delta = -1; // Will be switched at start -1 = down, 1 = up
    for (x = SKY_GRID_WIDTH -1; x >= SKY_GRID_WIDTH/2; x--) // this is really too grids
    {
        y_delta *= -1; // Swap direction, starts high
        y_start = (y_delta == 1 ? 0 : SKY_GRID_HEIGHT - 1);
        y_cnt = 0;

        for (y = y_start; y_cnt < SKY_GRID_HEIGHT; y += y_delta, ++y_cnt )
        {
            skyGrid->setPixal(x,y, network[port]->getRGB(channel));
            channel += 3;
            pixels_in_string++;
            if (pixels_in_string >= 1000)
            {
                pixels_in_string = 0;
                channel = 0;
                ++port;
            }
            if (channel >= 510)
            {
                channel = 0;
                ++port;
            }
        }
    }
    printf("DEBUG: Sky Right: Port: %d, channel: %d\n", port, channel);

    //Snowman left aka Road
    y_delta = -1; // Will be switched at start -1 = down, 1 = up
    port = 78; // Array is zero based. Port = 178
    channel = 0; // also zero based.
    pixels_in_string = 0;
    for (x = 0; x < SNOWMEN_WIDTH; x++)
    {
        y_delta *= -1; // Swap direction
        y_start = (y_delta == 1 ? 0 : SNOWMEN_HEIGHT - 1);
        y_cnt = 0;

        for (y = y_start; y_cnt < SNOWMEN_HEIGHT; y += y_delta, ++y_cnt )
        {
            snowmen[SNOWMAN_LEFT]->setPixal(x,SNOWMEN_HEIGHT-1 - y, network[port]->getRGB(channel));
            //printf("X: %d, Y: %d, port: %d, channel: %d\n", x,y,port,channel);
            channel += 3;
            pixels_in_string++;
            if (pixels_in_string >= 1000)
            {
                pixels_in_string = 0;
                channel = 0;
                if (port > 78) // really 178
                {
                    port = 72; // really 172
                }
                else if (port > 72) // really 172
                {
                    port = 66; // really 166
                }
                else if (port > 66)
                {
                    port = 60;
                }
                else if (port > 60)
                {
                    port = 54;
                }
                else if (port < 54)
                {
                    printf("Error settng up Left snowman.\n");
                    throw "Error setting up Left Snowmen.";
                }
                else
                {
                    ++port;
                }
            }
            if (channel >= 510)
            {
                channel = 0;
                ++port;
            }
        }
    }
    printf("SNOWMAN LEFT FINAL: X: %d, Y: %d, port: %d, channel: %d\n", x,y,port,channel);


    //Splash left aka Road
    y_delta = 1; // Will be switched at start -1 = down, 1 = up

    for (x = 0; x < SPLASH_GRID_WIDTH; x++)
    {
        y_delta *= -1; // Swap direction
        y_start = (y_delta == 1 ? 0 : SPLASH_GRID_HEIGHT - 1);
        y_cnt = 0;

        for (y = y_start; y_cnt < SPLASH_GRID_HEIGHT; y += y_delta, ++y_cnt )
        {
            splash[SNOWMAN_LEFT]->setPixal(x,SPLASH_GRID_HEIGHT-1 - y, network[port]->getRGB(channel));
            //printf("Splash: X: %d, Y: %d, port: %d, channel: %d\n", x,y,port,channel);
            channel += 3;
            pixels_in_string++;
            if (pixels_in_string >= 1000)
            {
                pixels_in_string = 0;
                channel = 0;
                if (port > 60) // really 160
                {
                    port = 54; // really 154
                }
                else if (port > 54) // really 154
                {
                    port = 48; // really 138
                }
                else if (port < 48)
                {
                    printf("Error settng up Left Splash.\n");
                    throw "Error setting up Left Splash.";
                }
                else
                {
                    ++port;
                }
            }
            if (channel >= 510)
            {
                channel = 0;
                ++port;
            }
        }
    }
    printf("SPLASH LEFT FINAL: X: %d, Y: %d, port: %d, channel: %d\n", x,y,port,channel);

    /*
     * Last of SKy
     */

    y_delta = 1; // Will be switched at start -1 = down, 1 = up

    for (x = 0 ; x < SKY_GRID_WIDTH/2; x++)
    {
        y_delta *= -1; // Swap direction
        y_start = (y_delta == 1 ? 0 : SKY_GRID_HEIGHT - 1);
        y_cnt = 0;

        for (y = y_start; y_cnt < SKY_GRID_HEIGHT; y += y_delta, ++y_cnt )
        {
            //printf("DEBUG: X: %d, Y: %d, port: %d, channel: %d\n", x,y,port,channel);
            skyGrid->setPixal(x,SKY_GRID_HEIGHT-1 - y, network[port]->getRGB(channel));
            channel += 3;
            pixels_in_string++;
            if (pixels_in_string >= 1000)
            {
                pixels_in_string = 0;
                channel = 0;
                if (port > 54) // really 160
                {
                    port = 48; // really 154
                }
                else if (port > 48) // really 154
                {
                    port = 42; // really +100
                }
                else if (port < 42)
                {
                    printf("Error settng up Left Splash.\n");
                    throw "Error setting up Left Splash.";
                }
                else
                {
                    ++port;
                }
            }
            if (channel >= 510)
            {
                channel = 0;
                ++port;
            }
        }
    }
    printf("sky Road: X: %d, Y: %d, port: %d, channel: %d\n", x,y,port,channel);

    RGBPicture::getAllPictures(); // Load all Pictures
    strcpy(message2, "Starting up");
}

Snowmen::~Snowmen()
{
    //dtor
}

int SnowmenGrid::getPos(int x, int y)
{
    int xmod = x%2;
    int pos = 0;

    if (xmod == 0)
    {
        pos = x * gridHeight + y;
    }
    else
    {
        pos = x*gridHeight + (gridHeight-1) - y;
    }
    return pos;
}

void SnowmenGrid::setPixal(int x, int y, RGBLight * p)
{
    pixals[getPos(x,y)] = p;
}

RGBLight *SnowmenGrid::getPixal(int x, int y)
{
    return pixals[getPos(x,y)];
}

RGBLight *SnowmenGrid::getBoard(int x, int y)
{
    int pos = y*gridWidth + x;
    return board[pos];
}


SnowmenGrid::SnowmenGrid(int width_, int height_, int dummy_with_, int dummy_height) : GenericGrid(width_,height_,dummy_with_,dummy_height)
{
// Setup Dummy Pials
    for (int i = 0 ; i < (height_ * width_); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
    }
}

SnowmenGrid *  Snowmen::getSnowmen(int pos)
{
    if (pos != SNOWMAN_RIGHT && pos != SNOWMAN_LEFT)
    {
        throw "Invalid position";
    }
    return snowmen[pos];
}

SnowmenGrid *  Snowmen::getSkyGrid()
{
    return skyGrid;
}

SnowmenGrid *  Snowmen::getSplashGrid(int pos)
{
    if (pos != SNOWMAN_RIGHT && pos != SNOWMAN_LEFT)
    {
        throw "Invalid position";
    }
    return splash[pos];
}

pthread_mutex_t*  Snowmen::getLock()
{
    return &lock;
}

void Snowmen::lockSnowmen()
{
    pthread_mutex_lock(&lock);
}

void Snowmen::releaseSnowmen()
{
    pthread_mutex_unlock(&lock);
}

void Snowmen::duckSnowman(int pos)
{
    int pos_y[] = {8,10,12,14,16,18,20,22,24,26, 28, 30, 32, 32, 32, 32,32,32,32,32};
    int duck[] = {1,2,3,4,5,6,7,8,9,8, 7,6,5,4,3,2,1,0};
    strcpy(message2, "Duck Snowman");
    SnowmenGrid *who;

    if (pos == SNOWMAN_LEFT)
    {
        who = getSnowmen(SNOWMAN_RIGHT);
        throwLeft(true);
        do_middle(pos,8,1,8,SNOWBALL_DURATION);
        ball_line(this->getSplashGrid(SNOWMAN_RIGHT), 0, 1, SPLASH_GRID_WIDTH, 6, BALL_SIZE_2IN);
        int x = 0;
        for (int i = 0; i< 18; i++, x=x+2)
        {
            lockSnowmen();
            snowmen[SNOWMAN_RIGHT]->setBackground(RGBColor::BLACK);
            drawSnowmen(SNOWMAN_RIGHT,hatStatus[SNOWMAN_RIGHT], duck[i]);
            who->drawCircle(x*2,pos_y[i],BALL_SIZE_1IN,RGBColor::WHITE);
            releaseSnowmen();
            write_data(SNOWBALL_DURATION);
            who->drawCircle(x*2,pos_y[i],BALL_SIZE_1IN,RGBColor::BLACK);
        }
    }
    else
    {
        int x = 0;
        who = getSnowmen(SNOWMAN_LEFT);
        throwRight(true);
        do_middle(pos,8,1,8,SNOWBALL_DURATION);
        ball_line(this->getSplashGrid(SNOWMAN_LEFT), SPLASH_GRID_WIDTH, 0, 0, 6, BALL_SIZE_2IN);
        for (int i = 0; i< 18; i++, x=x+2)
        {
            lockSnowmen();
            snowmen[SNOWMAN_LEFT]->setBackground(RGBColor::BLACK);
            drawSnowmen(SNOWMAN_LEFT,hatStatus[SNOWMAN_LEFT], duck[i]);
            who->drawCircle(SNOWMEN_WIDTH - x*2,pos_y[i],BALL_SIZE_1IN,RGBColor::WHITE);
            releaseSnowmen();
            write_data(SNOWBALL_DURATION);
            who->drawCircle(SNOWMEN_WIDTH - x*2,pos_y[i],BALL_SIZE_1IN,RGBColor::BLACK);
        }
    }
}

void Snowmen::drawMouth(int pos, double mouthPos)
{

    int x = 0;
    int y = 0;
    SnowmenGrid * who = getSnowmen(pos);

    if (mouthPos < 1)
        return ;

    // Mouth
    if (pos == SNOWMAN_LEFT)
    {
        x=SNOWMEN_WIDTH/2+8;
        y=SNOWMEN_HEIGHT-60;
        who->drawCircle(x-1,y-1,mouthPos,RGBColor::BLACK);
        who->drawCircle(x,y-1,mouthPos,RGBColor::BLACK);
    }
    else
    {
        x=SNOWMEN_WIDTH/2-8;
        y=SNOWMEN_HEIGHT-60;

        if (who_right == Trump)
        {
            x -= 4;
            y+=22;
        }
        else if (who_right == Biden)
        {
            x -= 2;
            y+=24;
            mouthPos *= 1.5;
        }


        who->drawCircle(x+1,y-1,mouthPos,RGBColor::BLACK);
        who->drawCircle(x,y-1,mouthPos,RGBColor::BLACK);
    }

}

void Snowmen::eatSnowball(int pos)
{
    int pos_y[] = {32,33,34,34,36,38,100,100,100,100, 100, 100, 100, 100, 32, 32,32,32,32,32};
    strcpy(message2, "Eat Snowball");
    SnowmenGrid *who;
    double mouth = 0;

    if (pos == SNOWMAN_LEFT)
    {
        int end_splash_y = 16;
        who = getSnowmen(SNOWMAN_RIGHT);

        // Drump has a lower mouth
        if (who_right == Trump)
        {
            end_splash_y = 24;
            for (int i =0; i < 18; i++)
            {
                pos_y[i] += 20;
            }
        }
        else if (who_right == Biden)
        {
            end_splash_y = 24;
            for (int i =0; i < 18; i++)
            {
                pos_y[i] += 20;
            }
        }

        throwLeft(true);
        do_middle(pos,10,6,16,SNOWBALL_DURATION);
        if (who_right)
            ball_line(this->getSplashGrid(SNOWMAN_RIGHT), 0, 8, SPLASH_GRID_WIDTH, end_splash_y, BALL_SIZE_2IN);
        int x = 0;

        for (int i = 0; i< 18; i++, x=x+2)
        {
            mouth = (i+1) * .3;
            if (i > 8)
            {
                mouth = (17-i) * .3;
            }
            lockSnowmen();
            snowmen[SNOWMAN_RIGHT]->setBackground(RGBColor::BLACK);
            drawSnowmen(SNOWMAN_RIGHT,hatStatus[SNOWMAN_RIGHT]);
            drawMouth(SNOWMAN_RIGHT,mouth);
            who->drawCircle(x*2,pos_y[i],BALL_SIZE_1IN,RGBColor::WHITE);
            releaseSnowmen();
            write_data(SNOWBALL_DURATION);
        }
    }
    else
    {
        int x = 0;
        who = getSnowmen(SNOWMAN_LEFT);
        throwRight(true);
        do_middle(pos,10,6,16,SNOWBALL_DURATION);
        ball_line(this->getSplashGrid(SNOWMAN_LEFT), SPLASH_GRID_WIDTH, 8, 0, 16, BALL_SIZE_2IN);
        for (int i = 0; i< 18; i++, x=x+2)
        {
            mouth = (i+1) * .3;
            if (i > 8)
            {
                mouth = (17-i) * .3;
            }

            lockSnowmen();
            snowmen[SNOWMAN_LEFT]->setBackground(RGBColor::BLACK);
            drawSnowmen(SNOWMAN_LEFT,hatStatus[SNOWMAN_LEFT]);
            drawMouth(SNOWMAN_LEFT,mouth);
            who->drawCircle(SNOWMEN_WIDTH - x*2,pos_y[i],BALL_SIZE_1IN,RGBColor::WHITE);
            releaseSnowmen();
            write_data(SNOWBALL_DURATION);
        }
    }
    getSkyGrid()->writeText(RGBColor::GREEN,16,0,"YUM!", false);
    write_data(2.0);
    getSkyGrid()->setBackground(RGBColor::BLACK);

}


void Snowmen::test_snowmen()
{
    if(0)
    {
        this->run();
    }

    if(0)
    {
        getSplashGrid(SNOWMAN_LEFT)->setBackground(RGBColor::BLUE);
        skyGrid->setBackground(RGBColor::PURPLE);
        skyGrid->getPixal(0,0)->set(RGBColor::GREEN);
        skyGrid->getPixal(SKY_GRID_WIDTH-1,SKY_GRID_HEIGHT-1)->set(RGBColor::RED);
        write_data(5.0);
    }

    if(1)
    {
        strcpy(message2, "Align Snowman");
        // Alignment lines
        getSnowmen(SNOWMAN_RIGHT)->setBackground(RGBColor::BLUE);
        getSnowmen(SNOWMAN_LEFT)->setBackground(RGBColor::BLUE);
        getSplashGrid(SNOWMAN_LEFT)->setBackground(RGBColor::BLUE);
        getSplashGrid(SNOWMAN_RIGHT)->setBackground(RGBColor::BLUE);
        skyGrid->setBackground(RGBColor::BLUE);

        for (int x = 0; x < SKY_GRID_WIDTH; x++)
        {
            skyGrid->getPixal(x,10)->set(RGBColor::WHITE); // 10 is top
            skyGrid->getPixal(x,SKY_GRID_HEIGHT-1)->set(RGBColor::WHITE); // 10 is top
        }

        for (int x =0; x < SPLASH_GRID_WIDTH; x++)
        {
            splash[SNOWMAN_LEFT]->getPixal(x,0)->set(RGBColor::WHITE);
            splash[SNOWMAN_RIGHT]->getPixal(x,0)->set(RGBColor::WHITE);
            splash[SNOWMAN_LEFT]->getPixal(x,13)->set(RGBColor::WHITE);
            splash[SNOWMAN_RIGHT]->getPixal(x,13)->set(RGBColor::WHITE);
        }

        write_data(5.0);
    }


    if(0)
    {
        strcpy(message2, "Test Vertical Lines");
        // test vertical lines
        getSnowmen(SNOWMAN_RIGHT)->setBackground(RGBColor::BLUE);
        getSnowmen(SNOWMAN_LEFT)->setBackground(RGBColor::BLUE);
        getSplashGrid(SNOWMAN_RIGHT)->setBackground(RGBColor::BLUE);
        skyGrid->setBackground(RGBColor::BLUE);

        getSplashGrid(SNOWMAN_LEFT)->setBackground(RGBColor::BLUE);

        write_data(2.0);
        for (int x = SKY_GRID_WIDTH-1; x >= 0; x--)
        {
            RGBColor *color = ((x%2) == 0 ? RGBColor::GREEN : RGBColor::RED);
            for (int y = 0; y < SNOWMEN_HEIGHT; y++)
            {
                if (x < SNOWMEN_WIDTH)
                {
                    getSnowmen(SNOWMAN_LEFT)->getPixal(x,y)->set(color);
                    getSnowmen(SNOWMAN_RIGHT)->getPixal(x,y)->set(color);
                }
                if (x < SPLASH_GRID_WIDTH && y < SPLASH_GRID_HEIGHT)
                {
                    getSplashGrid(SNOWMAN_LEFT)->getPixal(x,y)->set(color);
                    getSplashGrid(SNOWMAN_RIGHT)->getPixal(x,y)->set(color);
                }
                if (x < SKY_GRID_WIDTH && y < SKY_GRID_HEIGHT)
                {
                    skyGrid->getPixal(x,y)->set(color);
                }
                write_data(0.005);
            }
        }
        write_data(3.0);
    }
}

void Snowmen::setAllOff()
{
    // Turn things off
    getSplashGrid(SNOWMAN_LEFT)->setBackground(RGBColor::BLACK);
    getSplashGrid(SNOWMAN_RIGHT)->setBackground(RGBColor::BLACK);
    getSnowmen(SNOWMAN_LEFT)->setBackground(RGBColor::BLACK);
    getSnowmen(SNOWMAN_RIGHT)->setBackground(RGBColor::BLACK);
    getSkyGrid()->setBackground(RGBColor::BLACK);


}

void Snowmen::run()
{
    while(1)
    {
        if (timeinfo->isDebug())
        {
            test_snowmen();
            setAllOff();
        }
        else if (timeinfo->isDisplayHours())
        {
            do_it_snowmen();
        }
        else
        {
            sprintf(message2, "Sleeping during day as hour is %d", timeinfo->getHourOfDay());
            setAllOff();
            sleep(2);
        }
    }
}

void Snowmen::hitNose(int snowmen, int start_y)
{
    strcpy(message2, "Hit Nose");

    GenericGrid *grid = getSnowmen(snowmen);
    int pos_x = (snowmen == SNOWMAN_RIGHT ? 1 : SNOWMEN_WIDTH-1);
    int i;
    double dy = ((double)(SNOWMAN_NOSE_HEIGHT-1-start_y))/4;
    double pos_y = start_y;
    for (i =0; i < 4; i++)
    {
        grid->drawCircle(pos_x,pos_y, BALL_SIZE_1IN, RGBColor::WHITE);
        write_data(SNOWBALL_DURATION);
        grid->drawCircle(pos_x,(int)pos_y, BALL_SIZE_1IN, RGBColor::BLACK);
        pos_x += (snowmen == SNOWMAN_RIGHT ? 2 : -2);
        pos_y += dy;
    }

    ++noseBalls[snowmen];

    lockSnowmen();
    drawSnowmen(snowmen, hatStatus[snowmen]);
    releaseSnowmen();

}


// Fade snowball
void Snowmen::fadeNoseBalls()
{
    strcpy(message2, "Fade Nose Balls");

    int i = 100;
    while (i> -1)
    {
        RGBColor *c = new  RGBColor(i,i,i);
        noseBallColor = c;
        lockSnowmen();
        drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
        i -=5;
        noseBallColor = RGBColor::WHITE;
        delete c; // Memory Management
    }

    noseBalls[SNOWMAN_LEFT] = noseBalls[SNOWMAN_RIGHT] = 0;
    lockSnowmen();
    drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();
}


void Snowmen::placeHatBack(int pos)
{
    RGBPicture *hat[4];
    if (pos == SNOWMAN_LEFT)
    {
        hat[0] = RGBPicture::getPicture("small_hat_0g.png");
        hat[1] = RGBPicture::getPicture("small_hat_10g.png");
        hat[2] = RGBPicture::getPicture("small_hat_0g.png");
        hat[3] = RGBPicture::getPicture("small_hat_-10g.png");
    }
    else
    {
        hat[0] = RGBPicture::getPicture("small_hat_0.png");
        hat[1] = RGBPicture::getPicture("small_hat_10.png");
        hat[2] = RGBPicture::getPicture("small_hat_0.png");
        hat[3] = RGBPicture::getPicture("small_hat_-10.png");
    }
    GenericGrid *snowman = getSnowmen(pos);
    strcpy(message2, "Place Hat Back");


    int hatId = 0;

    int end_x= (pos == SNOWMAN_RIGHT ? 7 : (SNOWMEN_WIDTH/2-16));

    for (int y = -28; y < -1; y++)
    {
        snowman->showPictureNow(*hat[hatId], end_x, y, true);
        write_data(SNOWBALL_DURATION*3);
        lockSnowmen();
        snowman->setBackground(RGBColor::BLACK);
        drawSnowmen(pos,false);
        releaseSnowmen();
        if (++hatId > 3)
        {
            hatId = 0;
        }
    }
    hatStatus[pos] = true;
    lockSnowmen();
    drawSnowmen(pos,true);
    releaseSnowmen();


}

void Snowmen::hitHat(int pos)
{
    strcpy(message2, "Hit Hat");

    double duration = SNOWBALL_DURATION * 2.5;
    RGBPicture *hats[10];
    int pos_x[] = {7,10,13,16,19,21,25,28,31,34};
    int pos_y[] = {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,};
    if (pos == SNOWMAN_RIGHT)
    {
        hats[0] = RGBPicture::getPicture("small_hat_0.png");
        hats[1] = RGBPicture::getPicture("small_hat_10.png");
        hats[2] = RGBPicture::getPicture("small_hat_20.png");
        hats[3] = RGBPicture::getPicture("small_hat_30.png");
        hats[4] = RGBPicture::getPicture("small_hat_40.png");
        hats[5] = RGBPicture::getPicture("small_hat_50.png");
        hats[6] = RGBPicture::getPicture("small_hat_60.png");
        hats[7] = RGBPicture::getPicture("small_hat_70.png");
        hats[8] = RGBPicture::getPicture("small_hat_80.png");
        hats[9] = RGBPicture::getPicture("small_hat_90.png");
    }
    else
    {
        hats[0] = RGBPicture::getPicture("small_hat_0g.png");
        hats[1] = RGBPicture::getPicture("small_hat_-10g.png");
        hats[2] = RGBPicture::getPicture("small_hat_-20g.png");
        hats[3] = RGBPicture::getPicture("small_hat_-30g.png");
        hats[4] = RGBPicture::getPicture("small_hat_-40g.png");
        hats[5] = RGBPicture::getPicture("small_hat_-50g.png");
        hats[6] = RGBPicture::getPicture("small_hat_-60g.png");
        hats[7] = RGBPicture::getPicture("small_hat_-70g.png");
        hats[8] = RGBPicture::getPicture("small_hat_-80g.png");
        hats[9] = RGBPicture::getPicture("small_hat_-90g.png");
    }
    GenericGrid *snowman = getSnowmen(pos);

    for (int i = 0; i < 10; i++)
    {
        int x = (pos == SNOWMAN_RIGHT ? pos_x[i] : (SNOWMEN_WIDTH/2 - pos_x[i] - 10));
        lockSnowmen();
        snowman->setBackground(RGBColor::BLACK);
        drawSnowmen(pos, false);
        RGBPicture *hat = hats[i];
        snowman->showPictureNow(*hat, x, pos_y[i], true);
        releaseSnowmen();
        write_data(duration);
    }

    // cleanup
    lockSnowmen();
    snowman->setBackground(RGBColor::BLACK);
    drawSnowmen(pos, false);
    releaseSnowmen();
    hatStatus[pos] = false;
    write_data(duration);

}

void Snowmen::drawSnowmen(int pos, bool withHat, int offset)
{
    int x,y;
    int topOffset = 3*offset;
    int midOffset = 2*offset;
    GenericGrid * who = getSnowmen(pos);

    // TODO: Make this a varialbe
    if ((pos == SNOWMAN_LEFT) || (who_right == Snowman))
    {

        //TOP
        who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-65+topOffset,10,RGBColor::WHITE);
        //Mid
        who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-45+midOffset,14,RGBColor::WHITE);
        //Bottom
        who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-20+offset,20,RGBColor::WHITE);

        //Fix the one on edge points bottom
        who->getPixal(SNOWMEN_WIDTH/2-20,SNOWMEN_HEIGHT-19+offset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2-20,SNOWMEN_HEIGHT-21+offset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+20,SNOWMEN_HEIGHT-19+offset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+20,SNOWMEN_HEIGHT-21+offset)->set(RGBColor::WHITE);

        //Fix the one on edge points MID
        who->getPixal(SNOWMEN_WIDTH/2-14,SNOWMEN_HEIGHT-46+midOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2-14,SNOWMEN_HEIGHT-44+midOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+14,SNOWMEN_HEIGHT-46+midOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+14,SNOWMEN_HEIGHT-44+midOffset)->set(RGBColor::WHITE);

        //Fix the one on edge points TOP
        who->getPixal(SNOWMEN_WIDTH/2-10,SNOWMEN_HEIGHT-64+topOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2-10,SNOWMEN_HEIGHT-66+topOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+10,SNOWMEN_HEIGHT-64+topOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+10,SNOWMEN_HEIGHT-66+topOffset)->set(RGBColor::WHITE);

        who->getPixal(SNOWMEN_WIDTH/2-1,SNOWMEN_HEIGHT-75+topOffset)->set(RGBColor::WHITE);
        who->getPixal(SNOWMEN_WIDTH/2+1,SNOWMEN_HEIGHT-75+topOffset)->set(RGBColor::WHITE);
        //Nose
        int startX = SNOWMEN_WIDTH/2+8;
        int endX = SNOWMEN_WIDTH/2+16;
        y = SNOWMAN_NOSE_HEIGHT + topOffset;
        if (pos == SNOWMAN_RIGHT)
        {
            startX -=23;
            endX -=23;
        }


        for (x = startX; x < endX; x++)
        {
            who->getPixal(x,y)->set(RGBColor::ORANGE);
            who->getPixal(x,y-1)->set(RGBColor::ORANGE);
        }

        // Eye
        x = (pos==SNOWMAN_LEFT ? SNOWMEN_WIDTH/2+5 : SNOWMEN_WIDTH/2-4);
        who->drawCircle(x,SNOWMEN_HEIGHT-70 + topOffset,1,RGBColor::BLACK);

        //Buttons
        who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-26+ midOffset,1,RGBColor::BLACK);
        who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-38+ midOffset,1,RGBColor::BLACK);
        who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-50+ midOffset,1,RGBColor::BLACK);

        // Mouth
        if (pos == SNOWMAN_LEFT)
        {
            x=SNOWMEN_WIDTH/2+8;
            y=SNOWMEN_HEIGHT-60+ topOffset;
            who->getPixal(x,y)->set(RGBColor::BLACK);
            who->getPixal(x-1,y)->set(RGBColor::BLACK);
            who->getPixal(x-2,y-1)->set(RGBColor::BLACK);
            who->getPixal(x-3,y-2)->set(RGBColor::BLACK);
        }
        else
        {
            x=SNOWMEN_WIDTH/2-8;
            y=SNOWMEN_HEIGHT-60+ topOffset;
            who->getPixal(x,y)->set(RGBColor::BLACK);
            who->getPixal(x+1,y)->set(RGBColor::BLACK);
            who->getPixal(x+2,y-1)->set(RGBColor::BLACK);
            who->getPixal(x+3,y-2)->set(RGBColor::BLACK);
        }
    }
    else if (who_right == Trump)
    {
        // use trump
        if (pos == SNOWMAN_RIGHT)
        {
            RGBPicture *pic = RGBPicture::getPicture("trump_left.png");
            if (pic)
            {
                who->showPictureNow(*pic,0,20+topOffset,true);
            }
        }
    }
    else if (who_right == Biden)
    {
        // use Biden
        if (pos == SNOWMAN_RIGHT)
        {
            RGBPicture *pic = RGBPicture::getPicture("biden_left.png");
            if (pic)
            {
                who->showPictureNow(*pic,6,20+topOffset,true);
            }
        }
    }


    if (withHat)
    {
        RGBPicture *hat = RGBPicture::getPicture("small_hat_0.png");
        if (pos == SNOWMAN_LEFT)
        {
            hat = RGBPicture::getPicture("small_hat_0g.png");
        }
        who->showPictureNow(*hat, 7, -2 + topOffset, true);
    }



    // Snowball Nose L1
    if (noseBalls[pos] > 0)
    {
        x = (pos == SNOWMAN_RIGHT ? SNOWMEN_WIDTH - (SNOWMEN_WIDTH/2+14) : SNOWMEN_WIDTH/2+12);
        who->drawCircle(x, SNOWMAN_NOSE_HEIGHT-1 + topOffset, BALL_SIZE_1IN, noseBallColor);
    }

    if (noseBalls[pos] > 1)
    {
        x = (pos == SNOWMAN_RIGHT ? SNOWMEN_WIDTH - (SNOWMEN_WIDTH/2+18) : SNOWMEN_WIDTH/2+16);
        who->drawCircle(x, SNOWMAN_NOSE_HEIGHT-1 + topOffset, BALL_SIZE_1IN, noseBallColor);
    }

    // If fading then draw nose again.
    if (noseBallColor->getBlue() < 100)
    {

        int startX = SNOWMEN_WIDTH/2+8;
        int endX = SNOWMEN_WIDTH/2+16;
        y = SNOWMAN_NOSE_HEIGHT + topOffset;
        if (pos == SNOWMAN_RIGHT)
        {
            startX -=23;
            endX -=23;
        }
        for (x = startX; x < endX; x++)
        {
            who->getPixal(x,y)->set(RGBColor::ORANGE);
            who->getPixal(x,y-1)->set(RGBColor::ORANGE);
        }
    }



    drawGroundSnow(pos, RGBColor::WHITE);

}

void DrawLine(GenericGrid *grid, int startX, int endX, double mx, int b, RGBColor *color)
{
    int x,y;
    for (x = startX; x<=endX; x++)
    {
        y = mx*x + b;
        grid->getPixal(x,y)->set(color);
    }
}

void Snowmen::throwRight(bool loft)
{
    const int sholder_x = 28;
    const int sholder_y = 42;
    //const int ball_x[] = {12+30,12+30,11+30, 10+30, 9+30, 8+30, 8+30, 7+30, 7+30, 6+30, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 1};
    const int ball_x[] = {SNOWMEN_WIDTH-12,SNOWMEN_WIDTH-12,SNOWMEN_WIDTH-11, SNOWMEN_WIDTH-10, SNOWMEN_WIDTH-9, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-7, SNOWMEN_WIDTH-7, SNOWMEN_WIDTH-6, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-10, SNOWMEN_WIDTH-12, SNOWMEN_WIDTH-14, SNOWMEN_WIDTH-16, SNOWMEN_WIDTH-19, SNOWMEN_WIDTH-22, SNOWMEN_WIDTH-25, SNOWMEN_WIDTH-28, SNOWMEN_WIDTH-32, SNOWMEN_WIDTH-36, SNOWMEN_WIDTH-40};
    const int ball_y[] = {16,16,16,16, 15, 15, 15, 14, 14, 13, 13, 12, 12, 11, 12, 12, 12, 12, 13, 14, 15, 17};
    const int elbow_x[] = {SNOWMEN_WIDTH-1,SNOWMEN_WIDTH-2,SNOWMEN_WIDTH-3, SNOWMEN_WIDTH-4, SNOWMEN_WIDTH-5, SNOWMEN_WIDTH-5, SNOWMEN_WIDTH-6, SNOWMEN_WIDTH-7, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-9};
    const int elbow_y[] = {30,30,30, 30, 30, 30, 30, 30, 30, 30};

    // For Arm away
    const int hand_x[] = {SNOWMEN_WIDTH-41, SNOWMEN_WIDTH-42, SNOWMEN_WIDTH-43, SNOWMEN_WIDTH-44, SNOWMEN_WIDTH-43, SNOWMEN_WIDTH-42};
    const int hand_y[] = {20, 26, 32, 38, 44, 50};
    const int ball2_x[] = {2,0, SPLASH_GRID_WIDTH-0, SPLASH_GRID_WIDTH-3, SPLASH_GRID_WIDTH-6, SPLASH_GRID_WIDTH-9, SPLASH_GRID_WIDTH-12, SPLASH_GRID_WIDTH-15, SPLASH_GRID_WIDTH-18, SPLASH_GRID_WIDTH-21, SPLASH_GRID_WIDTH-23};
    int ball2_y[] = {17,17, 8, 8, 8, 8,  8,  8,  8, 8, 8};


    GenericGrid *right = getSnowmen(SNOWMAN_RIGHT);
    GenericGrid *splash = getSplashGrid(SNOWMAN_RIGHT);

    if (loft)
    {
        ball2_y[4] = 7;
        ball2_y[5] = 6;
        ball2_y[6] = 5;
        ball2_y[7] = 4;
        ball2_y[8] = 3;
        ball2_y[9] = 2;
        ball2_y[10] = 1;
    }

    // Pull back the Arm
    for (int i = 0; i< 10; i++)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);

        right->plotLineWidth(elbow_x[i],elbow_y[i],sholder_x,sholder_y,2.0,RGBColor::WHITE);
        right->plotLineWidth(elbow_x[i],elbow_y[i],ball_x[i],ball_y[i],2.0,RGBColor::WHITE);
        right->drawCircle(ball_x[i],ball_y[i],BALL_SIZE_1IN, RGBColor::WHITE);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    // Move the Arm foward as a Straight line
    for (int i = 10; i< 22; i++)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);

        right->plotLineWidth(sholder_x,sholder_y,ball_x[i],ball_y[i],2.0,RGBColor::WHITE);
        right->drawCircle(ball_x[i],ball_y[i],BALL_SIZE_1IN, RGBColor::WHITE);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    // Move Arm Away
    for (int i = 0; i< 6; i++)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);

        right->plotLineWidth(sholder_x,sholder_y,hand_x[i],hand_y[i],2.0,RGBColor::WHITE);
        if (i < 2)
        {
            right->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_1IN, RGBColor::WHITE);
        }
        else
        {
            //printf("DEBUG: Splash %d as %d, %d\n", i, ball2_x[i], ball2_y[i]);
            splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::WHITE);
        }
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
        if (i>=2)
        {
            splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::BLACK);
        }
    }

    // Clean up Snowmen
    lockSnowmen();
    right->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();

    // Throw slpash
    for (int i = 6; i < 11; i++)
    {
        splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::WHITE);
        write_data(SNOWBALL_DURATION);
        splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::BLACK);
    }
}

void Snowmen::throwLeft(bool loft)
{
    const int sholder_x = 16;
    const int sholder_y = 42;
    const int ball_x[] = {12,12,11, 10, 9, 8, 8, 7, 7, 6, 8, 10, 12, 14, 16, 19, 22, 25, 28, 32, 36, 40};
    const int ball_y[] = {16,16,16,16, 15, 15, 15, 14, 14, 13, 13, 12, 12, 11, 12, 12, 12, 12, 13, 14, 15, 17};
    const int elbow_x[] = {1,2,3, 4, 5, 5, 6, 7, 8, 9};
    const int elbow_y[] = {30,30,30, 30, 30, 30, 30, 30, 30, 30};

    // For Arm away
    const int hand_x[] = {41, 42, 43, 44, 43, 42};
    const int hand_y[] = {20, 26, 32, 38, 44, 50};
    const int ball2_x[] = {44,48, 0, 3, 6, 9, 12, 15, 18, 21, 23};
    int ball2_y[] = {17,17, 8, 8, 8, 8,  8,  8,  8, 8, 8};

    if (loft)
    {
        ball2_y[4] = 7;
        ball2_y[5] = 6;
        ball2_y[6] = 5;
        ball2_y[7] = 4;
        ball2_y[8] = 3;
        ball2_y[9] = 2;
        ball2_y[10] = 1;
    }

    GenericGrid *left = getSnowmen(SNOWMAN_LEFT);
    GenericGrid *splash = getSplashGrid(SNOWMAN_LEFT);

    // Pull back the Arm
    for (int i = 0; i< 10; i++)
    {
        lockSnowmen();
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);

        left->plotLineWidth(elbow_x[i],elbow_y[i],sholder_x,sholder_y,2.0,RGBColor::WHITE);
        left->plotLineWidth(elbow_x[i],elbow_y[i],ball_x[i],ball_y[i],2.0,RGBColor::WHITE);
        left->drawCircle(ball_x[i],ball_y[i],BALL_SIZE_1IN, RGBColor::WHITE);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    // Move the Arm foward as a Straight line
    for (int i = 10; i< 22; i++)
    {
        lockSnowmen();
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);

        left->plotLineWidth(sholder_x,sholder_y,ball_x[i],ball_y[i],2.0,RGBColor::WHITE);
        left->drawCircle(ball_x[i],ball_y[i],BALL_SIZE_1IN, RGBColor::WHITE);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    // Move Arm Away
    for (int i = 0; i< 6; i++)
    {
        lockSnowmen();
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);

        left->plotLineWidth(sholder_x,sholder_y,hand_x[i],hand_y[i],2.0,RGBColor::WHITE);
        if (i < 2)
        {
            left->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_1IN, RGBColor::WHITE);
        }
        else
        {
            splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::WHITE);
        }
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
        if (i>=2)
        {
            splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::BLACK);
        }
    }

    // Clean up Snowmen
    lockSnowmen();
    left->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
    releaseSnowmen();

    // Throw slpash
    for (int i = 6; i < 11; i++)
    {
        splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::WHITE);
        write_data(SNOWBALL_DURATION);
        splash->drawCircle(ball2_x[i],ball2_y[i],BALL_SIZE_2IN, RGBColor::BLACK);
    }
}

void Snowmen::do_middle(int start_snowmen, int start_y, int high_y, int end_y, double duration)
{
    int x = 0;
    int dx = 2;
    if (start_snowmen == SNOWMAN_RIGHT)
    {
        dx = -2;
        x = SKY_GRID_WIDTH - 1;
    }
    // GOing up
    double dy = ((double)(high_y-start_y)) / ((double)SKY_GRID_WIDTH/4);
    double y = (double) start_y;

    // First Half
    for (int i = 0; i < SKY_GRID_WIDTH/4; i++)
    {
        //printf("DEBUG: %d, %d, %f\n", x, (int)y, dy1);
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, RGBColor::WHITE);
        write_data(duration);
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, RGBColor::BLACK);
        x += dx;
        y += dy;
    }

    //Second Half
    y = high_y;
    dy = ((double)(end_y-high_y)) / ((double)SKY_GRID_WIDTH/4);
    for (int i = 0; i <= SKY_GRID_WIDTH/4; i++)
    {
        //printf("DEBUG: %d, %d, %f\n", x, (int)y, dy1);
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, RGBColor::WHITE);
        write_data(duration);
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, RGBColor::BLACK);
        x += dx;
        y += dy;
    }

}

// Used to have ball come across Splash gird
void Snowmen::ball_line(GenericGrid *grid, int start_x, int start_y, int end_x, int end_y, double ballSize)
{
    int dx = (end_x > start_x) ? 1: -1;
    double dy = ((double)(end_y-start_y)) / abs(start_x-end_x);

    int x = start_x;
    double y = (double)start_y;
    while (x != end_x)
    {
        grid->drawCircle(x,(int)y,ballSize, RGBColor::WHITE);
        //printf("ball: %d, %d\n", x, (int)y);
        write_data(SNOWBALL_DURATION);
        grid->drawCircle(x,(int)y,ballSize, RGBColor::BLACK);
        x += dx;
        y += dy;
    }
}

void Snowmen::throwLeftStickNose(bool arch)
{
    strcpy(message2, "Throw Left Nose");

    throwLeft(arch);
    if (arch)
    {
        do_middle(SNOWMAN_LEFT,8,1,8,SNOWBALL_DURATION);
    }
    else
    {
        do_middle(SNOWMAN_LEFT,18,14,18 -1,SNOWBALL_DURATION);
    }
    ball_line(this->getSplashGrid(SNOWMAN_RIGHT), 0, arch ? 0 : 8, SPLASH_GRID_WIDTH, 12, BALL_SIZE_2IN);
    hitNose(SNOWMAN_RIGHT, 24);
}

void Snowmen::throwRightStickNose(bool arch)
{
    strcpy(message2, "Throw Right Stick Nose");

    throwRight(arch);
    if (arch)
    {
        do_middle(SNOWMAN_RIGHT,8,1,8,SNOWBALL_DURATION);
    }
    else
    {
        do_middle(SNOWMAN_RIGHT,18,14,18 -1,SNOWBALL_DURATION);
    }
    ball_line(this->getSplashGrid(SNOWMAN_LEFT), SPLASH_GRID_WIDTH,  arch ? 0 : 8, 0, 12, BALL_SIZE_2IN);
    hitNose(SNOWMAN_LEFT, 24);
}

void Snowmen::throwHitHat(int pos)
{
    int pos_y[] = {10,10,11,11,12,12,13,13,14,14};
    strcpy(message2, "Throw Hit Hat");

    GenericGrid *hat;
    if (pos == SNOWMAN_LEFT)
    {
        hat = getSnowmen(SNOWMAN_RIGHT);
        throwLeft(true);
        do_middle(pos,8,1,8,SNOWBALL_DURATION);
        ball_line(this->getSplashGrid(SNOWMAN_RIGHT), 0, 0, SPLASH_GRID_WIDTH, 6, BALL_SIZE_2IN);
        for (int x = 0; x< 10; x=x+2)
        {
            hat->drawCircle(x*2,pos_y[x],BALL_SIZE_1IN,RGBColor::WHITE);
            write_data(SNOWBALL_DURATION);
            hat->drawCircle(x*2,pos_y[x],BALL_SIZE_1IN,RGBColor::BLACK);
        }
        hitHat(SNOWMAN_RIGHT);
    }
    else
    {
        hat = getSnowmen(SNOWMAN_LEFT);
        throwRight(true);
        do_middle(pos,8,1,8,SNOWBALL_DURATION);
        ball_line(this->getSplashGrid(SNOWMAN_LEFT), SPLASH_GRID_WIDTH, 0, 0, 6, BALL_SIZE_2IN);
        for (int x = 0; x< 10; x=x+2)
        {
            hat->drawCircle(SNOWMEN_WIDTH - x*2,pos_y[x],BALL_SIZE_1IN,RGBColor::WHITE);
            write_data(SNOWBALL_DURATION);
            hat->drawCircle(SNOWMEN_WIDTH - x*2,pos_y[x],BALL_SIZE_1IN,RGBColor::BLACK);
        }
        hitHat(SNOWMAN_LEFT);
    }

}

void Snowmen::throwGround(int pos, bool goHigh)
{
    GenericGrid *splash = getSplashGrid(pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT : SNOWMAN_LEFT);
    if (pos == SNOWMAN_LEFT)
    {
        throwLeft(goHigh);
    }
    else
    {
        throwRight(goHigh);
    }

    if (goHigh)
    {
        do_middle(pos,8,1,SKY_GRID_HEIGHT-5,SNOWBALL_DURATION);
        if (pos == SNOWMAN_LEFT)
        {
            splash->archBallOverTime(45,-22,50,0,26,BALL_SIZE_2IN,SNOWBALL_DURATION,RGBColor::WHITE);
        }
        else
        {
            splash->archBallOverTime(45,45,50,26,-2,BALL_SIZE_2IN,SNOWBALL_DURATION,RGBColor::WHITE);
        }
    }
    else
    {
        do_middle(pos,18,16,SKY_GRID_HEIGHT +1,SNOWBALL_DURATION);
        if (pos == SNOWMAN_LEFT)
        {
            splash->archBallOverTime(45,-22,55,0,26,BALL_SIZE_2IN,SNOWBALL_DURATION,RGBColor::WHITE);
        }
        else
        {
            splash->archBallOverTime(45,45,55,26,-2,BALL_SIZE_2IN,SNOWBALL_DURATION,RGBColor::WHITE);
        }
    }

    ++groundSnowLevel[pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT : SNOWMAN_LEFT];
    drawGroundSnow(pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT : SNOWMAN_LEFT, RGBColor::WHITE);

}

void Snowmen::drawGroundSnow(int pos, RGBColor *color)
{
    double level = groundSnowLevel[pos];
    int x,y;

    if (level <= 0)
    {
        return;
    }
    if (level > 4)
    {
        level = 4;
    }
    GenericGrid *splash = getSplashGrid(pos);
    GenericGrid *snowman = getSnowmen(pos);

    y = SNOWMEN_HEIGHT -2;
    x = pos== SNOWMAN_RIGHT ? 1 : SNOWMEN_WIDTH -2;
    snowman->drawCircle(x,y,BALL_SIZE_1IN * level, color);

    y= SPLASH_GRID_HEIGHT -1;
    x = pos== SNOWMAN_LEFT ? 1 : SPLASH_GRID_WIDTH -2;
    splash->drawCircle(x,y,BALL_SIZE_2IN * level,color);

}

void Snowmen::fadeSnow()
{
    strcpy(message2, "Fade Snow");

    while(groundSnowLevel[SNOWMAN_LEFT] > 0 || groundSnowLevel[SNOWMAN_RIGHT] > 0)
    {
        lockSnowmen();
        drawGroundSnow(SNOWMAN_LEFT, RGBColor::BLACK);
        drawGroundSnow(SNOWMAN_RIGHT, RGBColor::BLACK);
        groundSnowLevel[SNOWMAN_LEFT] += -0.1;
        groundSnowLevel[SNOWMAN_RIGHT] += -0.1;
        drawGroundSnow(SNOWMAN_LEFT, RGBColor::WHITE);
        drawGroundSnow(SNOWMAN_RIGHT, RGBColor::WHITE);
        drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION*3);
    }

    groundSnowLevel[SNOWMAN_LEFT] = 0;
    groundSnowLevel[SNOWMAN_RIGHT] = 0;

}

void Snowmen::cannonShot(int snowmen_pos)
{
    RGBPicture *cannon;
    GenericGrid *cannon_grid = getSplashGrid(snowmen_pos);
    GenericGrid *end_splash_Grid = getSplashGrid(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT);
    GenericGrid *end_snowman = getSnowmen(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT);
    int x,y,dx, splash_x, splash_y, snowman_x;
    double size_1in = CANNON_BALL_SIZE_1IN;
    static int dy[] = {24,22,20,19,18,17,16,15,14,13,12,11,11,10,10,9,9,8,8,8,7,7,7,6,6,6,5,5,5,5,5,6,6,6,7,7,7,8,8,9,9,10,10,11,11,12,12, 13, 13, 14, 14, 15, 15, 15, 15,15, 15,15,15,15,15,15,15,0,0,0,0,0,0,0};

    strcpy(message2, "Cannon Shot");

    if (snowmen_pos == SNOWMAN_LEFT)
    {
        cannon = RGBPicture::getPicture("cannon_right.png");
        dx=+2;
        x=0;
    }
    else
    {
        cannon = RGBPicture::getPicture("cannon_left.png");
        dx=-2;
        x=SKY_GRID_WIDTH;
    }

    /*
     * Drop the cannon
     */
    for (y=-30; y <=20; y++)
    {
        lockSnowmen();
        cannon_grid->setBackground(RGBColor::BLACK);
        drawGroundSnow(snowmen_pos, RGBColor::WHITE);
        cannon_grid->showPictureNow(*cannon,0,y,true);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    /*
     * Fire across sky
     */
    y=SKY_GRID_HEIGHT/2;
    for (int i = 0; i < SKY_GRID_WIDTH/2+SPLASH_GRID_WIDTH/2+13; i++)
    {
        y=dy[i];
        splash_x = i-44;
        splash_y = (i-42)/2;
        if (snowmen_pos == SNOWMAN_RIGHT)
        {
            splash_x = SPLASH_GRID_WIDTH - splash_x;
        }
        getSkyGrid()->drawCircle(x,y,CANNON_BALL_SIZE_2IN,RGBColor::WHITE);
        if (i > 44)
        {
            end_splash_Grid->drawCircle(splash_x,splash_y,CANNON_BALL_SIZE_2IN, RGBColor::WHITE);
        }
        if (i > 60)
        {
            snowman_x = (i-70)*2;
            if (snowmen_pos == SNOWMAN_RIGHT)
            {
                snowman_x = SNOWMEN_WIDTH - snowman_x;
            }
            end_snowman->drawCircle(snowman_x,splash_y*2,CANNON_BALL_SIZE_1IN, RGBColor::WHITE);
        }

        write_data(SNOWBALL_DURATION);
        getSkyGrid()->drawCircle(x,y,CANNON_BALL_SIZE_2IN,RGBColor::BLACK);
        if (i > 44)
        {
            end_splash_Grid->drawCircle(splash_x,splash_y,CANNON_BALL_SIZE_2IN, RGBColor::BLACK);
        }
        if (i > 60)
        {
            end_snowman->drawCircle(snowman_x,splash_y*2,CANNON_BALL_SIZE_1IN, RGBColor::BLACK);
        }
        x += dx;
    }

    /*
     * Do Explosition
     */

    splash_y = splash_y*2;
    for (int i = 0; i < 22; i++)
    {
        snowman_x += (dx/2);
        splash_y += 1;
        end_snowman->drawCircle(snowman_x,splash_y,size_1in,RGBColor::WHITE);
        write_data(SNOWBALL_DURATION);
        size_1in += 2.0;
    }
    // fill buttom
    end_snowman->drawCircle(SNOWMEN_HEIGHT,SNOWMEN_WIDTH/2,size_1in,RGBColor::WHITE);

    // to black
    snowman_x = (snowmen_pos == SNOWMAN_RIGHT ? SNOWMEN_WIDTH : 0);
    size_1in = 0;
    for (int i = 0; i < 38; i++)
    {
        end_snowman->drawCircle(snowman_x,-6,size_1in,RGBColor::BLACK);
        end_splash_Grid->drawCircle(SPLASH_GRID_WIDTH/2,SPLASH_GRID_HEIGHT,size_1in,RGBColor::BLACK);
        write_data(SNOWBALL_DURATION/2);
        size_1in += 3.0;
    }
    // Reset -- Destroyed on is the reverse!
    groundSnowLevel[snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT] = 0;
    noseBalls[snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT] = 0;
    hatStatus[snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT] = false;

    getSkyGrid()->writeText(RGBColor::GREEN,10,0,"GONE!", false);
    size_1in = 1;
    write_data(2.0);
    for (int i =0; i< 50; i++)
    {
        lockSnowmen();
        cannon_grid->drawCircle(SPLASH_GRID_WIDTH/2,SPLASH_GRID_HEIGHT/2,size_1in,RGBColor::BLACK);
        drawGroundSnow(snowmen_pos, RGBColor::WHITE);
        releaseSnowmen();
        skyGrid->drawCircle(SKY_GRID_WIDTH/2,SKY_GRID_HEIGHT/2,size_1in,RGBColor::BLACK);
        write_data(SNOWBALL_DURATION);
        size_1in += 1;
    }

    // If SNowman right is being redrawn (throw from left)
    // Use mqtt to find next snowman type
    if (snowmen_pos == SNOWMAN_LEFT)
    {
        // need to choose next right snowmen
        int who = mqtt->getSnowmanVote();
        std::string line1 = "Next Up";
        std::string name;
        switch(who)
        {
        case 1:
            who_right = Trump;
            name = "Mr. Trump!";
            getSkyGrid()->writeTextNew(RGBColor::WHITE,22,0, line1,false,12);
            gjhSleep(1);
            getSkyGrid()->writeTextNew(RGBColor::WHITE,14,12, name,false,12);
            break;
        case 2:
            who_right = Biden;
            name = "Mr. Biden!";
            getSkyGrid()->writeTextNew(RGBColor::WHITE,22,0,line1,false,12);
            gjhSleep(1);
            getSkyGrid()->writeTextNew(RGBColor::WHITE,14,12, name,false,12);
            break;
        default:
            who_right = Snowman;
            name = "Snowmen";
            break;
        };

        strcpy(message_who, name.c_str());
    }



    // reset
    lockSnowmen();
    drawSnowmen(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT, false);
    releaseSnowmen();
    placeHatBack(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT);

    if (who_right != Snowman)
    {
        gjhSleep(2);
        getSkyGrid()->setBackground(RGBColor::BLACK);
    }


}

void Snowmen::do_it_snowmen()
{
    lockSnowmen();
    drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();

    // Debug area
    while(0)
    {
        strcpy(message2, "DEBUG MODE");
        throwGround(SNOWMAN_RIGHT, true);
        throwGround(SNOWMAN_RIGHT, false);
        throwGround(SNOWMAN_RIGHT, true);
        write_data(1.0);
        throwRightStickNose(true);
        write_data(1.0);
        throwRightStickNose(false);
        throwHitHat(SNOWMAN_RIGHT);
        write_data(2.0);
        this->cannonShot(SNOWMAN_LEFT);
        write_data(2.0);
        fadeSnow();
        fadeNoseBalls();
    }

    bool didSomething = true;
    int id = -1;

    /*
     * Find the next option
     * Some options are not valid if not using snowman
     */

    while (id < 0 )
    {
        id = rand()%15;
        if (who_right != Snowman)
        {
            if (id < 4 || id == 6 || id ==7 ||id == 8 || id ==13 )
            {
                id = -1;
            }
        }
    }

    int chance = 0;
    switch(id)
    {
    case 0:
        if (noseBalls[SNOWMAN_LEFT]>=2 || noseBalls[SNOWMAN_RIGHT] >=2)
        {
            fadeNoseBalls();
        }
        else
        {
            throwLeftStickNose(false);
        }
        break;
    case 1:
        if (noseBalls[SNOWMAN_LEFT]>=2 || noseBalls[SNOWMAN_RIGHT] >=2)
        {
            fadeNoseBalls();
        }
        else
        {
            throwLeftStickNose(true);
        }
        break;
    case 2:
        if (noseBalls[SNOWMAN_LEFT]>=2 || noseBalls[SNOWMAN_RIGHT] >=2)
        {
            fadeNoseBalls();
        }
        else
        {
            throwRightStickNose(false);
        }
        break;
    case 3:
        if (noseBalls[SNOWMAN_LEFT]>=2 || noseBalls[SNOWMAN_RIGHT] >=2)
        {
            fadeNoseBalls();
        }
        else
        {
            throwRightStickNose(true);
        }
        break;
    case 4:
        if (groundSnowLevel[SNOWMAN_RIGHT] >= 3)
        {
            fadeSnow();
        }
        else
        {
            throwGround(SNOWMAN_LEFT, true);
        }
        break;
    case 5:
        if (groundSnowLevel[SNOWMAN_RIGHT] >= 3)
        {
            fadeSnow();
        }
        else
        {
            throwGround(SNOWMAN_LEFT, false);
        }
        break;
    case 6:
        if (groundSnowLevel[SNOWMAN_LEFT] >= 3)
        {
            fadeSnow();
        }
        else
        {

            throwGround(SNOWMAN_RIGHT, true);
        }
        break;
    case 7:
        if (groundSnowLevel[SNOWMAN_LEFT] >= 3)
        {
            fadeSnow();
        }
        else
        {
            throwGround(SNOWMAN_RIGHT, false);
        }
        break;
    case 8:
        if (hatStatus[SNOWMAN_LEFT] == true)
        {
            throwHitHat(SNOWMAN_RIGHT);
        }
        else
        {
            placeHatBack(SNOWMAN_LEFT);
        }
        break;
    case 9:
        if (hatStatus[SNOWMAN_RIGHT] == true)
        {
            throwHitHat(SNOWMAN_LEFT);
        }
        else
        {
            placeHatBack(SNOWMAN_RIGHT);
        }
        break;
    case 10:
        chance = rand()%4;
        if (chance == 0)
            cannonShot(SNOWMAN_LEFT);
        else
            didSomething = false;
        break;
    case 11:
        chance = rand()%4;
        if (chance == 0)
            cannonShot(SNOWMAN_RIGHT);
        else
            didSomething = false;
        break;
    case 12:
        chance = rand()%3;
        if (chance == 0)
            duckSnowman(SNOWMAN_LEFT);
        else
            didSomething = false;
        break;
    case 13:
        chance = rand()%3;
        if (chance == 0)
            duckSnowman(SNOWMAN_RIGHT);
        else
            didSomething = false;
        break;

    case 14:
        chance = rand()%2;
        if (who_right != Snowman)
        {
            chance = 0;
        }
        eatSnowball(chance == 0 ? SNOWMAN_LEFT : SNOWMAN_RIGHT);
        break;

    default:
        break;
    }

    // wait between
    if (didSomething)
    {
        id = rand()%11;
        switch(id)
        {
        case 0:
        case 1:
        case 2:
            write_data(0.2);
            break;
        case 3:
        case 4:
        case 5:
            write_data(0.8);
            break;
        case 6:
            // Do nothing
            break;
        case 7:
        case 8:
            write_data(1.0);
            break;
        case 9:
            write_data(1.5);
            break;
        case 10:
            write_data(2.5);
            break;
        default:
            break;
        }
    }


    //throwHitHat(SNOWMAN_LEFT);
    //placeHatBack(SNOWMAN_RIGHT);
    //write_data(1.0);
    //throwHitHat(SNOWMAN_RIGHT);
    //placeHatBack(SNOWMAN_LEFT);

    //hitHat(SNOWMAN_LEFT);
    //throwLeftStickNose(true);
    //hitHat(SNOWMAN_RIGHT);

    //throwLeftStickNose(false);
    //throwRightStickNose(true);
    //throwRightStickNose(false);

}


char * Snowmen::getMessage()
{
    return message2;
}

char * Snowmen::getMessageWho()
{
    return message_who;
}


