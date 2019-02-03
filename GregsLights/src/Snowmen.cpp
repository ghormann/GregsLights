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

Snowmen::Snowmen(bool skipTime)
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        throw "Mutext init failed for Snowmen";
    }
    groundSnowLevel[SNOWMAN_LEFT] = 0;
    groundSnowLevel[SNOWMAN_RIGHT] = 0;
    hatStatus[SNOWMAN_LEFT] = true;
    hatStatus[SNOWMAN_RIGHT] = true;

    //ctor
    timeinfo = new TimeInfo(skipTime,false);
    snowman_t = 0;
    snowmen[0] = new SnowmenGrid(SNOWMEN_WIDTH,SNOWMEN_HEIGHT,SNOWMEN_WIDTH, SNOWMEN_HEIGHT,skipTime,false);
    snowmen[1] = new SnowmenGrid(SNOWMEN_WIDTH,SNOWMEN_HEIGHT,SNOWMEN_WIDTH, SNOWMEN_HEIGHT,skipTime,false);
    splash[0] = new SnowmenGrid(SPLASH_GRID_WIDTH,SPLASH_GRID_HEIGHT,SPLASH_GRID_WIDTH, SPLASH_GRID_HEIGHT,skipTime,false);
    splash[1] = new SnowmenGrid(SPLASH_GRID_WIDTH,SPLASH_GRID_HEIGHT,SPLASH_GRID_WIDTH, SPLASH_GRID_HEIGHT,skipTime,false);
    skyGrid = new SnowmenGrid(SKY_GRID_WIDTH,SKY_GRID_HEIGHT,SKY_GRID_WIDTH, SKY_GRID_HEIGHT,skipTime,false);
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


SnowmenGrid::SnowmenGrid(int width_, int height_, int dummy_with_, int dummy_height, bool skipTime, bool newYears) : GenericGrid(width_,height_,dummy_with_,dummy_height,skipTime,newYears)
{
    // Setup Dummy Pials
    for (int i = 0 ; i < (height_ * width_); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());

// TODO (ghormann#1#): Fix Mapping of Pixals
        this->pixals[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
        this->pixals[i]->set(0,0,0);
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

void Snowmen::lockSnowmen()
{
    pthread_mutex_lock(&lock);
}

void Snowmen::releaseSnowmen()
{
    pthread_mutex_unlock(&lock);
}

void Snowmen::run()
{
    while(1)
    {
        if (timeinfo->isDisplayHours())
        {
            do_it_snowmen();
        }
        else
        {
            sprintf(message2, "Sleeping during day as hour is %d", timeinfo->getHourOfDay());
            //setBodies(false);
            sleep(60);
        }
    }
}

void Snowmen::hitNose(int snowmen, int start_y)
{
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
    pos_x = (snowmen == SNOWMAN_RIGHT ? SNOWMEN_WIDTH - (SNOWMEN_WIDTH/2+16) : SNOWMEN_WIDTH/2+16);
    grid->drawCircle(pos_x, SNOWMAN_NOSE_HEIGHT-1, BALL_SIZE_1IN, RGBColor::WHITE);
    write_data(1.0);
    // Fade snowball
    i = 100;
    while (i> -1)
    {
        // RGBColor is proper C++ for Memory Mangement.
        RGBColor c =  RGBColor(i,i,i);
        grid->drawCircle(pos_x, SNOWMAN_NOSE_HEIGHT-1, BALL_SIZE_1IN, &c);
        write_data(SNOWBALL_DURATION);
        i -=10;

    }

    drawSnowmen(snowmen, hatStatus[snowmen]);

}

void Snowmen::placeHatBack(int pos)
{
    RGBPicture *hat[4];
    hat[0] = RGBPicture::getPicture("small_hat_0.png");
    hat[1] = RGBPicture::getPicture("small_hat_10.png");
    hat[2] = RGBPicture::getPicture("small_hat_0.png");
    hat[3] = RGBPicture::getPicture("small_hat_-10.png");
    GenericGrid *snowman = getSnowmen(pos);

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
    drawSnowmen(pos,true);


}

void Snowmen::hitHat(int pos)
{
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
        hats[0] = RGBPicture::getPicture("small_hat_0.png");
        hats[1] = RGBPicture::getPicture("small_hat_-10.png");
        hats[2] = RGBPicture::getPicture("small_hat_-20.png");
        hats[3] = RGBPicture::getPicture("small_hat_-30.png");
        hats[4] = RGBPicture::getPicture("small_hat_-40.png");
        hats[5] = RGBPicture::getPicture("small_hat_-50.png");
        hats[6] = RGBPicture::getPicture("small_hat_-60.png");
        hats[7] = RGBPicture::getPicture("small_hat_-70.png");
        hats[8] = RGBPicture::getPicture("small_hat_-80.png");
        hats[9] = RGBPicture::getPicture("small_hat_-90.png");
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

void Snowmen::drawSnowmen(int pos, bool withHat)
{
    int x,y;
    GenericGrid * who = getSnowmen(pos);
    //TOP
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-65,10,RGBColor::WHITE);
    //Mid
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-45,14,RGBColor::WHITE);
    //Bottom
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-20,20,RGBColor::WHITE);

    if (withHat)
    {
        RGBPicture *hat = RGBPicture::getPicture("small_hat_0.png");
        who->showPictureNow(*hat, 7, -2, true);
    }


    //Nose
    int startX = SNOWMEN_WIDTH/2+8;
    int endX = SNOWMEN_WIDTH/2+16;
    y = SNOWMAN_NOSE_HEIGHT;
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
    who->drawCircle(x,SNOWMEN_HEIGHT-70,1,RGBColor::BLACK);
    //Buttons
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-26,1,RGBColor::BLACK);
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-38,1,RGBColor::BLACK);
    who->drawCircle(SNOWMEN_WIDTH/2,SNOWMEN_HEIGHT-50,1,RGBColor::BLACK);

    // Mouth
    if (pos == SNOWMAN_LEFT)
    {
        x=SNOWMEN_WIDTH/2+8;
        y=SNOWMEN_HEIGHT-60;
        who->getPixal(x,y)->set(RGBColor::BLACK);
        who->getPixal(x-1,y)->set(RGBColor::BLACK);
        who->getPixal(x-2,y-1)->set(RGBColor::BLACK);
        who->getPixal(x-3,y-2)->set(RGBColor::BLACK);
    }
    else
    {
        x=SNOWMEN_WIDTH/2-8;
        y=SNOWMEN_HEIGHT-60;
        who->getPixal(x,y)->set(RGBColor::BLACK);
        who->getPixal(x+1,y)->set(RGBColor::BLACK);
        who->getPixal(x+2,y-1)->set(RGBColor::BLACK);
        who->getPixal(x+3,y-2)->set(RGBColor::BLACK);
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
        write_data(SNOWBALL_DURATION);
        grid->drawCircle(x,(int)y,ballSize, RGBColor::BLACK);
        x += dx;
        y += dy;
    }
}

void Snowmen::throwLeftStickNose(bool arch)
{
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

void Snowmen::do_it_snowmen()
{
    drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    write_data(10.0);
    while(1)
    {

        throwGround(SNOWMAN_LEFT, true);
        write_data(1.0);
        throwGround(SNOWMAN_LEFT, false);
        write_data(1.0);
        throwHitHat(SNOWMAN_LEFT);
        write_data(1.0);
        throwLeftStickNose(false);
        throwGround(SNOWMAN_LEFT, true);
        throwGround(SNOWMAN_RIGHT, true);

        write_data(2.0);


        fadeSnow();
        write_data(1.0);
        placeHatBack(SNOWMAN_RIGHT);

        drawSnowmen(SNOWMAN_LEFT,hatStatus[SNOWMAN_LEFT]);
        drawSnowmen(SNOWMAN_RIGHT,hatStatus[SNOWMAN_RIGHT]);


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

        write_data(1.0);
    }
    //sleep(5); // Replace me
}


char * Snowmen::getMessage()
{
    return message2;
}

