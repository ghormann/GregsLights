#include "../include/Snowmen.h"
#include "../include/controller/DummyBulb.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "RGBPicture.h"
#include "../include/GarageSign.h"
#include <json/json.h>
#include <strings.h>


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
    createSnowmanPictures();
    snprintf(message_who, sizeof(message_who), "%s", who_right->name.c_str());

}

Snowmen::~Snowmen()
{
    //dtor
}

void Snowmen::createSnowmanPictures()
{
    RGBPicture::getAllPictures(); // Load all Pictures

    SnowmanPicture *regular = new SnowmanPicture();
    availSnowman.push_back(regular);
    this->who_right = regular;

    SnowmanPicture *who = new SnowmanPicture();
    who->pic_offset_x = 0;
    who->pic_offset_y = 20;
    who->splash_end_y = 24;
    who->splash_offset_y= 20;
    who->pic_offset_y = 20;
    who->mouth_offset_x = -4;
    who->mouth_offset_y = 22;
    who->name_offset_x = 14;
    who->name = "Mr. Trump";
    who->pic = RGBPicture::getPicture("trump_left.png");
    //availSnowman.push_back(who);

    who = new SnowmanPicture();
    who->pic_offset_x = 6;
    who->pic_offset_y = 20;
    who->splash_end_y = 24;
    who->splash_offset_y= 20;
    who->mouth_offset_x = -2;
    who->mouth_offset_y = 24;
    who->mouth_multiplier = 1.5;
    who->name_offset_x = 14;
    who->name = "Mr. Biden";
    who->pic = RGBPicture::getPicture("biden_left.png");
    //availSnowman.push_back(who);

    who = new SnowmanPicture();
    who->pic_offset_x = -2;
    who->pic_offset_y = 24;
    who->splash_end_y = 16;
    who->splash_offset_y= 12;
    who->mouth_offset_x = 4;
    who->mouth_offset_y = 14;
    who->name_offset_x = 10;
    who->name = "Hans Gruber";
    who->catchphrase = "Now I have a snowball. Ho Ho Ho.";
    who->signature = SIG_FALL;
    who->signatureText = "Yippee-ki-yay!";
    who->pic = RGBPicture::getPicture("hans_gruber.png");
    availSnowman.push_back(who);


    who = new SnowmanPicture();
    who->pic_offset_x = -4;
    who->pic_offset_y = 22;
    who->splash_end_y = 18;
    who->splash_offset_y= 12;
    who->mouth_offset_x = 5;
    who->mouth_offset_y = 16;
    who->name = "Mary";
    who->show_misses = false;
    who->name_offset_x = 29;
    who->pic = RGBPicture::getPicture("mary_left.png");
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -8;
    who->pic_offset_y = 20;
    who->splash_end_y = 18;
    who->splash_offset_y= 12;
    who->mouth_offset_x = 0;
    who->mouth_offset_y = 17;
    who->mouth_multiplier = 1.5;
    who->name_offset_x = 28;
    who->name = "Emily";
    who->show_misses = false;
    who->pic = RGBPicture::getPicture("emily_left.png");
    availSnowman.push_back(who);

    who = new SnowmanPicture();
    who->pic_offset_x = 1;
    who->pic_offset_y = 20;
    who->splash_end_y = 22;
    who->splash_offset_y= 16;
    who->mouth_offset_x = -7;
    who->mouth_offset_y = 19;
    who->mouth_multiplier = 1.2;
    who->name_offset_x = 30;
    who->name = "Matt";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("matt_left.png");
    availSnowman.push_back(who);
    //this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = 7;
    who->pic_offset_y = 17;
    who->splash_end_y = 22;
    who->splash_offset_y= 16;
    who->mouth_offset_x = 4;
    who->mouth_offset_y = 18;
    who->mouth_multiplier = 1.2;
    who->name_offset_x = 6;
    who->name = "Buddy the Elf";
    who->catchphrase = "SANTA! I know him!";
    who->signature = SIG_EAT_EVERYTHING;
    who->signatureText = "Candy, candy canes, candy corn!";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("buddy_left.png");
    availSnowman.push_back(who);
    //this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -4;
    who->pic_offset_y = 17;
    who->splash_end_y = 22;
    who->splash_offset_y= 20;
    who->mouth_offset_x = -2;
    who->mouth_offset_y = 22;
    who->mouth_multiplier = 1.2;
    who->name_offset_x = 6;
    who->name = "Red Among Us";
    who->catchphrase = "Red looks sus...";
    who->signature = SIG_EJECT;
    who->signatureText = "Red was not The Impostor.";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("among_us.png");
    availSnowman.push_back(who);
    //this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -4;
    who->pic_offset_y = 20;
    who->splash_end_y = 16;
    who->splash_offset_y= 10;
    who->mouth_offset_x = 7;
    who->mouth_offset_y = 17;
    who->mouth_multiplier = 1.2;
    who->name_offset_x = 4;
    who->name = "Clark Griswold";
    who->catchphrase = "Fun old-fashioned family Christmas!";
    who->signature = SIG_TWINKLE_LIGHTS;
    who->signatureText = "Hallelujah!";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("clark_left.png");
    availSnowman.push_back(who);
    //this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -22;
    who->pic_offset_y = 20;
    who->splash_end_y = 24;
    who->splash_offset_y= 22;
    who->mouth_offset_x = 0;
    who->mouth_offset_y = 25;
    who->mouth_multiplier = 1.1;
    who->name_offset_x = 8;
    who->name = "Cousin Eddie";
    who->catchphrase = "Surprised to see us, Clark?";
    who->show_misses = false;
    who->pic = RGBPicture::getPicture("cousin_eddie.png");
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -6;
    who->pic_offset_y = 10;
    who->splash_end_y = 24;
    who->splash_offset_y= 22;
    who->mouth_offset_x = 0;
    who->mouth_offset_y = 20;
    who->mouth_multiplier = 1.1;
    who->name_offset_x = 16;
    who->name = "Tom Nook";
    who->catchphrase = "Hm? Yes, yes!";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("tom_nook.png");
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -2;
    who->pic_offset_y = 20;
    who->splash_end_y = 24;
    who->splash_offset_y= 22;
    who->mouth_offset_x = -7;
    who->mouth_offset_y = 20;
    who->mouth_multiplier = 1.5;
    who->name_offset_x = 4;
    who->name = "Charlie Brown";
    who->catchphrase = "Good grief!";
    who->signature = SIG_FALL;
    who->signatureText = "AAUGH!";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("charlie_brown.png");
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -2;
    who->pic_offset_y = 20;
    who->splash_end_y = 24;
    who->splash_offset_y= 22;
    who->mouth_offset_x = 0;
    who->mouth_offset_y = 25;
    who->mouth_multiplier = 1.5;
    who->name_offset_x = 28;
    who->name = "Sonic";
    who->catchphrase = "Gotta go fast!";
    who->signature = SIG_DASH;
    who->signatureText = "Too slow!";
    who->speed = 1.5;
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("sonic_side_small.png");
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = -3;
    who->pic_offset_y = 20;
    who->splash_end_y = 24;
    who->splash_offset_y= 22;
    who->mouth_offset_x = -10;
    who->mouth_offset_y = 25;
    who->mouth_multiplier = 1.5;
    who->name_offset_x = 30;
    who->name = "Tails";
    who->catchphrase = "Wait for me, Sonic!";
    who->signature = SIG_FLY;
    who->signatureText = "Look, I can fly!";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("tails_left.png");
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = 1;
    who->pic_offset_y = 8;
    who->splash_end_y = 16;
    who->splash_offset_y= 11;
    who->mouth_offset_x = -1;
    who->mouth_offset_y = 15;
    who->mouth_multiplier = 1.2;
    who->name_offset_x = 8;
    who->name = "The Grinch";
    who->catchphrase = "Stink. Stank. Stunk.";
    who->signature = SIG_STEAL_HAT;
    who->signatureText = "Your hat is MINE!";
    who->show_misses = false;
    who->pic = RGBPicture::getPicture("grinch_left.png");
    availSnowman.push_back(who);
    //this->who_right = who;

    who = new SnowmanPicture();
    who->pic_offset_x = 0;
    who->pic_offset_y = 0;
    who->splash_end_y = 18;
    who->splash_offset_y= 13;
    who->mouth_offset_x = 2;
    who->mouth_offset_y = 17;
    who->mouth_multiplier = 1.2;
    who->name_offset_x = 24;
    who->name = "Minion";
    who->catchphrase = "Bello!";
    who->signature = SIG_BANANA;
    who->signatureText = "BANANA!";
    who->show_misses = true;
    who->pic = RGBPicture::getPicture("minion_left.png");
    availSnowman.push_back(who);
    this->who_right = who;

    useTestCharacter();
    animationSpeed = who_right->speed;
}

/*
 * For testing a character: GREGSLIGHTS_SNOWMAN="The Grinch" ./GregsLights -t
 * keeps that character up and does its signature move every other move.
 */
void Snowmen::useTestCharacter()
{
    const char *name = getenv("GREGSLIGHTS_SNOWMAN");
    if (name == NULL || *name == 0)
    {
        return;
    }
    for (SnowmanPicture *p : availSnowman)
    {
        if (strcasecmp(p->name.c_str(), name) == 0)
        {
            testCharacter = p;
            who_right = p;
            printf("Snowman test mode: always %s\n", p->name.c_str());
            return;
        }
    }
    printf("GREGSLIGHTS_SNOWMAN: no character named %s\n", name);
}

void Snowmen::setGarageSign(GarageSign *g)
{
    this->garageSign = g;
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

        // Handle offsets
        x += who_right->mouth_offset_x;
        y += who_right->mouth_offset_y;
        mouthPos *= who_right->mouth_multiplier;

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

        // Handle offsets
        if (who_right->pic != NULL)
        {
            end_splash_y = who_right->splash_end_y;
            for (int i =0; i < 18; i++)
            {
                pos_y[i] += who_right->splash_offset_y;
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
        publishMqtt();
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
    if (pos == SNOWMAN_RIGHT && hatStolen)
    {
        // Knocking the stolen hat off the Grinch
        hats[0] = RGBPicture::getPicture("small_hat_0g.png");
        hats[1] = RGBPicture::getPicture("small_hat_10g.png");
        hats[2] = RGBPicture::getPicture("small_hat_20g.png");
        hats[3] = RGBPicture::getPicture("small_hat_30g.png");
        hats[4] = RGBPicture::getPicture("small_hat_40g.png");
        hats[5] = RGBPicture::getPicture("small_hat_50g.png");
        hats[6] = RGBPicture::getPicture("small_hat_60g.png");
        hats[7] = RGBPicture::getPicture("small_hat_70g.png");
        hats[8] = RGBPicture::getPicture("small_hat_80g.png");
        hats[9] = RGBPicture::getPicture("small_hat_90g.png");
    }
    else if (pos == SNOWMAN_RIGHT)
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
    if (pos == SNOWMAN_RIGHT)
    {
        hatStolen = false;
    }
    write_data(duration);

}

/*
 * xShift/yShift move the right character (and its hat) by pixels.
 * Only used by the signature moves.
 */
void Snowmen::drawSnowmen(int pos, bool withHat, int offset, int xShift, int yShift)
{
    int x,y;
    int topOffset = 3*offset;
    int midOffset = 2*offset;
    GenericGrid * who = getSnowmen(pos);

    if ((pos == SNOWMAN_LEFT) || (who_right->pic == NULL))
    {
        xShift = yShift = 0;  // Only pictures move

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
    else
    {
        RGBPicture *pic = who_right->pic;
        who->showPictureNow(*pic, who_right->pic_offset_x+xShift, who_right->pic_offset_y+topOffset+yShift,true);
    }

    if (withHat)
    {
        RGBPicture *hat = RGBPicture::getPicture("small_hat_0.png");
        if (pos == SNOWMAN_LEFT || hatStolen)
        {
            hat = RGBPicture::getPicture("small_hat_0g.png");
        }
        who->showPictureNow(*hat, 7 + xShift, -2 + topOffset + yShift, true);
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

void Snowmen::do_middle(int start_snowmen, int start_y, int high_y, int end_y, double duration, RGBColor *color)
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
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, color);
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
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, color);
        write_data(duration);
        skyGrid->drawCircle(x,(int)y, BALL_SIZE_2IN, RGBColor::BLACK);
        x += dx;
        y += dy;
    }

}

// Used to have ball come across Splash gird
void Snowmen::ball_line(GenericGrid *grid, int start_x, int start_y, int end_x, int end_y, double ballSize, RGBColor *color)
{
    int dx = (end_x > start_x) ? 1: -1;
    double dy = ((double)(end_y-start_y)) / abs(start_x-end_x);

    int x = start_x;
    double y = (double)start_y;
    while (x != end_x)
    {
        grid->drawCircle(x,(int)y,ballSize, color);
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

    if (pos == SNOWMAN_RIGHT && !(this->who_right->show_misses))
    {
        return;
    }

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
    int x,y,dx, splash_x, splash_y, snowman_x = 0;
    double size_1in = CANNON_BALL_SIZE_1IN;
    static int dy[] = {24,22,20,19,18,17,16,15,14,13,12,11,11,10,10,9,9,8,8,8,7,7,7,6,6,6,5,5,5,5,5,6,6,6,7,7,7,8,8,9,9,10,10,11,11,12,12, 13, 13, 14, 14, 15, 15, 15, 15,15, 15,15,15,15,15,15,15,0,0,0,0,0,0,0,0,0,0,0,0};

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
    resetSnowmen(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT);

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
        updateSnowmanFromMQTT();
        publishMqtt(true); // Reset snowmen vote
        snowmanStepCount = 0;
        signatureCount = 0;
        animationSpeed = who_right->speed;
        if (who_right->pic != NULL)
        {
            std::string line1 = "Next Up";
            getSkyGrid()->writeTextNew(RGBColor::WHITE,22,0, line1,false,12);
            gjhSleep(1);
            getSkyGrid()->writeTextNew(RGBColor::WHITE,who_right->name_offset_x,12, who_right->name,false,12);
        }

        publishMqtt();
        snprintf(message_who, sizeof(message_who), "%s", who_right->name.c_str());
    }



    // reset
    lockSnowmen();
    drawSnowmen(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT, false);
    releaseSnowmen();
    placeHatBack(snowmen_pos == SNOWMAN_LEFT ? SNOWMAN_RIGHT: SNOWMAN_LEFT);

    if (who_right->pic != NULL)
    {
        gjhSleep(2);
        getSkyGrid()->setBackground(RGBColor::BLACK);
    }

    if (snowmen_pos == SNOWMAN_LEFT)
    {
        if (!who_right->catchphrase.empty())
        {
            skyText(who_right->catchphrase, RGBColor::GREEN);
            gjhSleep(2.5);
            getSkyGrid()->setBackground(RGBColor::BLACK);
        }

        // The Grinch may have left with our hat
        if (!hatStatus[SNOWMAN_LEFT])
        {
            placeHatBack(SNOWMAN_LEFT);
        }
    }
}

void Snowmen::resetSnowmen(int pos) {
    groundSnowLevel[pos] = 0;
    noseBalls[pos] = 0;
    hatStatus[pos] = false;
    if (pos == SNOWMAN_RIGHT)
    {
        hatStolen = false;
    }
}

void Snowmen::updateSnowmanFromMQTT()
{
    if (testCharacter != NULL)
    {
        who_right = testCharacter;
        return;
    }

    int who = mqtt->getSnowmanVote();
    mqtt->clearSnowmanVote();
    if (who >= (int)availSnowman.size())
    {
        who = 0;
    }
    if (who == 0)
    {
        if (++nonSpecialSnowmanCount > 5)
        {
            // If the Snowman has been standard for a long time then
            // force change.
            who = rand()% ((int)availSnowman.size());
            nonSpecialSnowmanCount = 0;
        }
    } else {
        nonSpecialSnowmanCount = 0;
    }
    who_right = availSnowman.at(who);
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
    bool characterMove = true;  /* False when just cleaning up the scene */
    int id = -1;

    /*
     * Make sure the signature move happens early, then sometimes after that
     */
    if (who_right->pic != NULL && who_right->signature != SIG_NONE)
    {
        int chance = (testCharacter != NULL ? 2 : SNOWMAN_SIGNATURE_CHANCE);
        if ((signatureCount == 0 && snowmanStepCount >= SNOWMAN_SIGNATURE_BY_MOVE)
                || rand() % chance == 0)
        {
            id = 15;
        }
    }

    /*
     * Find the next option
     * Some options are not valid if not using snowman
     */

    while (id < 0 )
    {
        id = rand()%15;
        if (who_right->pic != NULL)
        {
            if (id < 4 || id == 6 || id ==7 ||id == 8 || id ==13 )
            {
                id = -1;
            }
        }
    }

    // Buddy eats everything thrown at him
    if (who_right->signature == SIG_EAT_EVERYTHING && (id == 4 || id == 5 || id == 12))
    {
        id = 14;
    }

    int stepLimit = mqtt->hasSnowmanVote() ? SNOWMAN_STEPS_VOTE_WAITING : SNOWMAN_STEPS;
    if (snowmanStepCount >= stepLimit)
    {
        cannonShot(SNOWMAN_LEFT);
        // Need to return, other wise something invalid can occur
        return;
    }
    snprintf(message_who, sizeof(message_who), "%s (%d/%d)", who_right->name.c_str(), snowmanStepCount, stepLimit);

    //id = 14;

    int chance = 0;
    switch(id)
    {
    case 0:
        if (noseBalls[SNOWMAN_LEFT]>=2 || noseBalls[SNOWMAN_RIGHT] >=2)
        {
            fadeNoseBalls();
            characterMove = false;
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
            characterMove = false;
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
            characterMove = false;
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
            characterMove = false;
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
            characterMove = false;
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
            characterMove = false;
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
            characterMove = false;
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
            characterMove = false;
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
        // Replaces the right snowman, so give it a chance to do something first
        chance = rand()%4;
        if (chance == 0 && snowmanStepCount >= SNOWMAN_MIN_MOVES)
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
        if (who_right->pic != NULL)
        {
            chance = 0;
        }
        eatSnowball(chance == 0 ? SNOWMAN_LEFT : SNOWMAN_RIGHT);
        break;

    case 15:
        signatureMove();
        break;

    default:
        break;
    }

    // Only count things the snowmen actually did. cannonShot(SNOWMAN_LEFT)
    // already reset the count for the new snowman.
    if (didSomething && characterMove && id != 10)
    {
        ++snowmanStepCount;
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

/*
 * The current character's special move
 */
void Snowmen::signatureMove()
{
    snprintf(message2, sizeof(message2), "Signature: %s", who_right->name.c_str());
    ++signatureCount;

    switch (who_right->signature)
    {
    case SIG_STEAL_HAT:
        stealHat();
        break;
    case SIG_FALL:
        characterFall();
        break;
    case SIG_FLY:
        characterFly();
        break;
    case SIG_DASH:
        characterDash();
        break;
    case SIG_EJECT:
        characterEject();
        break;
    case SIG_BANANA:
        bananaThrow();
        break;
    case SIG_EAT_EVERYTHING:
        eatSnowball(SNOWMAN_LEFT);
        skyText(who_right->signatureText, RGBColor::GREEN);
        write_data(2.0);
        getSkyGrid()->setBackground(RGBColor::BLACK);
        break;
    case SIG_TWINKLE_LIGHTS:
        twinkleLights();
        break;
    default:
        break;
    }
}

/*
 * Centered text on the sky. Long text is split into two lines.
 */
void Snowmen::skyText(const std::string &text, RGBColor *color)
{
    GenericGrid *sky = getSkyGrid();
    std::string line1 = text;
    std::string line2 = "";
    int fontSize = 12;

    if (sky->getTextWidthNew(text, fontSize) > SKY_GRID_WIDTH)
    {
        // Split at the space nearest the middle, preferring one after punctuation
        int middle = text.size() / 2;
        int best = -1;
        int bestScore = 0;
        for (int i = 1; i < (int)text.size(); i++)
        {
            if (text[i] != ' ')
                continue;
            int score = abs(i - middle);
            if (strchr(".,!?", text[i-1]) != NULL)
                score -= text.size() / 4;
            if (best < 0 || score < bestScore)
            {
                best = i;
                bestScore = score;
            }
        }
        if (best > 0)
        {
            line1 = text.substr(0, best);
            line2 = text.substr(best + 1);
            fontSize = 10;  // Two lines only fit in the sky at this size
        }
    }

    while (fontSize > 8 && (sky->getTextWidthNew(line1, fontSize) > SKY_GRID_WIDTH
                            || (!line2.empty() && sky->getTextWidthNew(line2, fontSize) > SKY_GRID_WIDTH)))
    {
        fontSize--;
    }

    sky->setBackground(RGBColor::BLACK);
    int x = (SKY_GRID_WIDTH - sky->getTextWidthNew(line1, fontSize)) / 2;
    if (line2.empty())
    {
        sky->writeTextNew(color, (x < 0 ? 0 : x), 6, line1, false, fontSize);
    }
    else
    {
        sky->writeTextNew(color, (x < 0 ? 0 : x), 2, line1, false, fontSize);
        x = (SKY_GRID_WIDTH - sky->getTextWidthNew(line2, fontSize)) / 2;
        sky->writeTextNew(color, (x < 0 ? 0 : x), 13, line2, false, fontSize);
    }
}

/*
 * Move the right character from one pixel offset to another.
 * trail draws speed lines behind it.
 */
void Snowmen::slideCharacter(int fromX, int fromY, int toX, int toY, int frames, double delay, RGBColor *trail)
{
    GenericGrid *right = getSnowmen(SNOWMAN_RIGHT);
    for (int i = 0; i <= frames; i++)
    {
        int x = fromX + (toX - fromX) * i / frames;
        int y = fromY + (toY - fromY) * i / frames;
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        if (trail != NULL)
        {
            // Characters face left, so behind is to the right when moving left
            int start = (toX < fromX ? x + 40 : x - 30);
            for (int line = 0; line < 4; line++)
            {
                int lineY = 35 + line * 14 + y;
                right->plotLineWidth(start, lineY, start + 25, lineY, 2.0, trail);
            }
        }
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT], 0, x, y);
        releaseSnowmen();
        write_data(delay);
    }
}

/*
 * Moves a picture (or a streak if pic is NULL) across one grid.
 * pos is which side the grid is on, so it can be redrawn behind it.
 */
void Snowmen::flyAcross(GenericGrid *grid, int pos, RGBPicture *pic, RGBColor *streak, int fromX, int toX, int y, int step)
{
    for (int x = fromX; (step > 0 ? x <= toX : x >= toX); x += step)
    {
        lockSnowmen();
        grid->setBackground(RGBColor::BLACK);
        if (grid == snowmen[pos])
        {
            drawSnowmen(pos, hatStatus[pos]);
        }
        else if (grid == splash[pos])
        {
            drawGroundSnow(pos, RGBColor::WHITE);
        }

        if (pic != NULL)
        {
            grid->showPictureNow(*pic, x, y, true);
        }
        else
        {
            // Blur with a ball at the front
            grid->plotLineWidth(x, y, x + 20, y, 4.0, streak);
            grid->drawCircle(step < 0 ? x : x + 20, y, 3, streak);
        }
        releaseSnowmen();
        write_data(0.02);
    }

    // Leave it clean
    lockSnowmen();
    grid->setBackground(RGBColor::BLACK);
    if (grid == snowmen[pos])
    {
        drawSnowmen(pos, hatStatus[pos]);
    }
    else if (grid == splash[pos])
    {
        drawGroundSnow(pos, RGBColor::WHITE);
    }
    releaseSnowmen();
}

/*
 * Draws a 1" picture at a position in inches across the whole yard
 * (see SNOWMAN_SKY_RAISE). On the 2" splash and sky grids each pixel
 * covers four picture pixels, so it shows at the right physical size.
 */
void Snowmen::drawPictureInYard(RGBPicture *pic, int x, int y)
{
    struct { GenericGrid *grid; int x0; int y0; int density; } areas[] =
    {
        {snowmen[SNOWMAN_LEFT], 0, 0, 1},
        {splash[SNOWMAN_LEFT], SNOWMEN_WIDTH, 0, 2},
        {skyGrid, SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*2, -SNOWMAN_SKY_RAISE, 2},
        {splash[SNOWMAN_RIGHT], SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*2 + SKY_GRID_WIDTH*2, 0, 2},
        {snowmen[SNOWMAN_RIGHT], SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*4 + SKY_GRID_WIDTH*2, 0, 1},
    };
    int picWidth, picHeight;
    pic->getSize(picWidth, picHeight);

    for (auto &area : areas)
    {
        if (area.density == 1)
        {
            area.grid->showPictureNow(*pic, x - area.x0, y - area.y0, true);
            continue;
        }

        int gw = area.grid->getGridWidth();
        int gh = area.grid->getGridHeight();
        for (int gx = 0; gx < gw; gx++)
        {
            int px = area.x0 + gx * 2 - x;   // Picture pixel under this light
            if (px < -1 || px >= picWidth)
                continue;
            for (int gy = 0; gy < gh; gy++)
            {
                int py = area.y0 + gy * 2 - y;
                if (py < -1 || py >= picHeight)
                    continue;

                // Average the lit picture pixels this light covers
                int r = 0, g = 0, b = 0, lit = 0;
                for (int dx = 0; dx < 2; dx++)
                {
                    for (int dy = 0; dy < 2; dy++)
                    {
                        int sx = px + dx, sy = py + dy;
                        if (sx < 0 || sy < 0 || sx >= picWidth || sy >= picHeight)
                            continue;
                        int pr, pg, pb;
                        pic->getRGB(sx, sy, pr, pg, pb);
                        if (pr || pg || pb)
                        {
                            r += pr; g += pg; b += pb; ++lit;
                        }
                    }
                }
                if (lit >= 2)
                {
                    area.grid->getPixal(gx, gy)->set(r/lit, g/lit, b/lit);
                }
            }
        }
    }
}

/*
 * Clears every grid back to the snowmen and ground snow
 */
void Snowmen::redrawYard(bool withRightHat)
{
    for (int pos = SNOWMAN_LEFT; pos <= SNOWMAN_RIGHT; pos++)
    {
        snowmen[pos]->setBackground(RGBColor::BLACK);
        splash[pos]->setBackground(RGBColor::BLACK);
        drawSnowmen(pos, pos == SNOWMAN_RIGHT ? withRightHat : hatStatus[pos]);
    }
    skyGrid->setBackground(RGBColor::BLACK);
}

/*
 * The Grinch lifts the left snowman's hat, it sails across the
 * yard and he wears it until he is replaced.
 */
void Snowmen::stealHat()
{
    strcpy(message2, "Steal Hat");

    if (!hatStatus[SNOWMAN_LEFT])
    {
        // Nothing left to steal
        skyText(who_right->catchphrase, RGBColor::GREEN);
        write_data(2.0);
        getSkyGrid()->setBackground(RGBColor::BLACK);
        return;
    }

    RGBPicture *hat = RGBPicture::getPicture("small_hat_0g.png");
    const int startX = 7;  // Where hats sit on a snowman grid
    const int endX = SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*4 + SKY_GRID_WIDTH*2 + 7;
    const int hatY = -2;

    // Out with the old hat
    if (hatStatus[SNOWMAN_RIGHT])
    {
        hitHat(SNOWMAN_RIGHT);
    }
    hatStatus[SNOWMAN_LEFT] = false;

    /*
     * The hat's lit part is 20x15 at (7,10) in the picture. Only the sky
     * has lights above the top of the other grids, so it moves level
     * while crossing between grids and only arcs up once it is entirely
     * over the sky. That way it never gets cut off.
     */
    const int level = -9;   // Lit part at the top row of the splash grids
    const int skyStart = SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*2 - 7;
    const int skyEnd = skyStart + SKY_GRID_WIDTH*2 - 20;
    const int peak = 14;    // How far it rises over the sky

    // Lift it off
    for (int y = hatY; y >= level; y--)
    {
        lockSnowmen();
        redrawYard(false);
        drawPictureInYard(hat, startX, y);
        releaseSnowmen();
        write_data(0.04);
    }

    // Across the yard
    for (int x = startX; x <= endX; x += 3)
    {
        int y = level;
        if (x > skyStart && x < skyEnd)
        {
            y -= (int)(peak * sin(M_PI * (x - skyStart) / (skyEnd - skyStart)));
        }
        lockSnowmen();
        redrawYard(false);
        drawPictureInYard(hat, x, y);
        releaseSnowmen();
        write_data(0.025);
    }

    // Put it on
    hatStolen = true;
    for (int y = level; y <= hatY; y++)
    {
        lockSnowmen();
        redrawYard(false);
        drawPictureInYard(hat, endX, y);
        releaseSnowmen();
        write_data(0.04);
    }

    hatStatus[SNOWMAN_RIGHT] = true;
    lockSnowmen();
    redrawYard(true);
    releaseSnowmen();

    skyText(who_right->signatureText, RGBColor::GREEN);
    write_data(2.0);
    getSkyGrid()->setBackground(RGBColor::BLACK);
}

/*
 * Slow motion fall off the bottom, then climb back up
 */
void Snowmen::characterFall()
{
    strcpy(message2, "Fall");
    skyText(who_right->signatureText, RGBColor::RED);
    slideCharacter(0, 0, 0, SNOWMEN_HEIGHT, 40, 0.06);
    write_data(1.0);
    getSkyGrid()->setBackground(RGBColor::BLACK);
    slideCharacter(0, SNOWMEN_HEIGHT, 0, 0, 15, 0.03);
}

/*
 * Fly up out of sight and float back down
 */
void Snowmen::characterFly()
{
    strcpy(message2, "Fly");
    skyText(who_right->signatureText, RGBColor::YELLOW);
    slideCharacter(0, 0, 0, -SNOWMEN_HEIGHT, 25, 0.03);
    write_data(1.0);
    slideCharacter(0, -SNOWMEN_HEIGHT, 0, 0, 30, 0.05);
    // Little bounce on landing
    slideCharacter(0, 0, 0, -4, 2, 0.08);
    slideCharacter(0, -4, 0, 0, 2, 0.08);
    getSkyGrid()->setBackground(RGBColor::BLACK);
}

/*
 * Zoom off, run a lap around the yard as a blue blur, and come back
 */
void Snowmen::characterDash()
{
    strcpy(message2, "Dash");

    // Off to the left
    slideCharacter(0, 0, -SNOWMEN_WIDTH - 20, 0, 6, 0.03, RGBColor::BLUE);

    // Past the left snowman low, back across the top of the sky
    flyAcross(getSplashGrid(SNOWMAN_RIGHT), SNOWMAN_RIGHT, NULL, RGBColor::BLUE, SPLASH_GRID_WIDTH, -16, 30, -4);
    flyAcross(getSkyGrid(), SNOWMAN_LEFT, NULL, RGBColor::BLUE, SKY_GRID_WIDTH, -16, 18, -4);
    flyAcross(getSplashGrid(SNOWMAN_LEFT), SNOWMAN_LEFT, NULL, RGBColor::BLUE, SPLASH_GRID_WIDTH, -16, 30, -4);
    flyAcross(getSnowmen(SNOWMAN_LEFT), SNOWMAN_LEFT, NULL, RGBColor::BLUE, SNOWMEN_WIDTH, -16, 85, -4);
    flyAcross(getSnowmen(SNOWMAN_LEFT), SNOWMAN_LEFT, NULL, RGBColor::BLUE, -16, SNOWMEN_WIDTH, 5, 4);
    flyAcross(getSplashGrid(SNOWMAN_LEFT), SNOWMAN_LEFT, NULL, RGBColor::BLUE, -16, SPLASH_GRID_WIDTH, 4, 4);
    flyAcross(getSkyGrid(), SNOWMAN_LEFT, NULL, RGBColor::BLUE, -16, SKY_GRID_WIDTH, 4, 4);
    flyAcross(getSplashGrid(SNOWMAN_RIGHT), SNOWMAN_RIGHT, NULL, RGBColor::BLUE, -16, SPLASH_GRID_WIDTH, 4, 4);

    // Back in from the right
    slideCharacter(SNOWMEN_WIDTH + 20, 0, 0, 0, 6, 0.03, RGBColor::BLUE);
    lockSnowmen();
    getSnowmen(SNOWMAN_RIGHT)->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();

    skyText(who_right->signatureText, RGBColor::BLUE);
    write_data(1.5);
    getSkyGrid()->setBackground(RGBColor::BLACK);
}

/*
 * Among Us ejection: drift off into space among the stars
 */
void Snowmen::characterEject()
{
    strcpy(message2, "Eject");
    GenericGrid *right = getSnowmen(SNOWMAN_RIGHT);
    int starX[30];
    int starY[30];
    for (int i = 0; i < 30; i++)
    {
        starX[i] = rand() % SNOWMEN_WIDTH;
        starY[i] = rand() % SNOWMEN_HEIGHT;
    }
    RGBColor *star = new RGBColor(40, 40, 40);

    skyText(who_right->signatureText, RGBColor::WHITE);
    for (int i = 0; i <= 70; i++)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        for (int j = 0; j < 30; j++)
        {
            right->getPixal(starX[j], starY[j])->set(star);
        }
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT], 0, i, -i/3);
        releaseSnowmen();
        write_data(0.05);
    }
    write_data(1.5);

    getSkyGrid()->setBackground(RGBColor::BLACK);
    slideCharacter(SNOWMEN_WIDTH + 10, 0, 0, 0, 20, 0.04);
    delete star;
}

/*
 * Throw a yellow snowball that splats on the left snowman's face
 */
void Snowmen::bananaThrow()
{
    strcpy(message2, "Banana Throw");
    GenericGrid *right = getSnowmen(SNOWMAN_RIGHT);
    GenericGrid *left = getSnowmen(SNOWMAN_LEFT);

    // Out of his hands and off to the left
    for (int x = 14; x >= 0; x -= 2)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
        right->drawCircle(x, 50 - (14 - x), BALL_SIZE_1IN, RGBColor::YELLOW);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION * 2);
    }
    lockSnowmen();
    right->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();

    ball_line(getSplashGrid(SNOWMAN_RIGHT), SPLASH_GRID_WIDTH, 8, 0, 4, BALL_SIZE_2IN, RGBColor::YELLOW);
    do_middle(SNOWMAN_RIGHT, 8, 1, 8, SNOWBALL_DURATION, RGBColor::YELLOW);
    ball_line(getSplashGrid(SNOWMAN_LEFT), SPLASH_GRID_WIDTH, 8, 0, 12, BALL_SIZE_2IN, RGBColor::YELLOW);

    // Into the face
    int noseX = SNOWMEN_WIDTH/2 + 12;
    int noseY = SNOWMAN_NOSE_HEIGHT - 1;
    for (int i = 0; i < 4; i++)
    {
        int x = SNOWMEN_WIDTH - 1 - i * (SNOWMEN_WIDTH - 1 - noseX) / 3;
        int y = 24 + i * (noseY - 24) / 3;
        lockSnowmen();
        left->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
        left->drawCircle(x, y, BALL_SIZE_1IN, RGBColor::YELLOW);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    // Splat
    for (int r = 2; r <= 7; r++)
    {
        left->drawCircle(noseX, noseY, r, RGBColor::YELLOW);
        write_data(0.03);
    }
    skyText(who_right->signatureText, RGBColor::YELLOW);
    write_data(1.5);

    lockSnowmen();
    left->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_LEFT, hatStatus[SNOWMAN_LEFT]);
    releaseSnowmen();
    getSkyGrid()->setBackground(RGBColor::BLACK);
}

/*
 * Clark plugs in: strings of twinkle lights wrap the left snowman and
 * hang across the yard, light up one by one, then chase and twinkle.
 * The garage sign joins in.
 */
void Snowmen::twinkleLights()
{
    strcpy(message2, "Twinkle Lights");
    RGBColor *colors[] = {RGBColor::RED, RGBColor::GREEN, RGBColor::BLUE, RGBColor::YELLOW, RGBColor::ORANGE, RGBColor::PURPLE};
    struct Light { GenericGrid *grid; int x; int y; int size; };
    std::vector<Light> lights;

    // Around the left snowman (same circles as drawSnowmen)
    struct { int y; int r; } balls[] = {{SNOWMEN_HEIGHT-65, 10}, {SNOWMEN_HEIGHT-45, 14}, {SNOWMEN_HEIGHT-20, 20}};
    for (auto &ball : balls)
    {
        int count = ball.r;  // More lights on the bigger balls
        for (int i = 0; i < count; i++)
        {
            double a = 2 * M_PI * i / count;
            int x = SNOWMEN_WIDTH/2 + (int)((ball.r - 1) * cos(a));
            int y = ball.y + (int)((ball.r - 1) * sin(a));
            if (y < SNOWMEN_HEIGHT - 1)
                lights.push_back({snowmen[SNOWMAN_LEFT], x, y, 1});
        }
    }
    // Hanging strings: across the splash grids and drooping along the sky
    for (int x = 1; x < SPLASH_GRID_WIDTH; x += 3)
    {
        int droop = (int)(2 * sin(M_PI * x / SPLASH_GRID_WIDTH));
        lights.push_back({splash[SNOWMAN_LEFT], x, 1 + droop, 0});
        lights.push_back({splash[SNOWMAN_RIGHT], x, 1 + droop, 0});
    }
    for (int x = 1; x < SKY_GRID_WIDTH; x += 3)
    {
        int droop = (int)(3 * fabs(sin(M_PI * x / 24.0)));
        lights.push_back({skyGrid, x, SKY_GRID_HEIGHT - 6 + droop, 0});
    }

    if (garageSign != NULL)
    {
        garageSign->requestHallelujah();
    }

    // Light up one by one as the plug goes in, then chase and twinkle
    int total = lights.size();
    for (int frame = 0; frame < 70; frame++)
    {
        int lit = (frame < 20 ? (frame + 1) * total / 20 : total);
        lockSnowmen();
        redrawYard(hatStatus[SNOWMAN_RIGHT]);
        if (frame >= 25)
        {
            skyText(who_right->signatureText, RGBColor::YELLOW);
        }
        for (int i = 0; i < lit; i++)
        {
            RGBColor *c = colors[(i + frame / 3) % 6];
            if (frame >= 20 && rand() % 12 == 0)
            {
                c = RGBColor::WHITE;  // Twinkle
            }
            lights[i].grid->drawCircle(lights[i].x, lights[i].y, lights[i].size, c);
        }
        releaseSnowmen();
        write_data(0.08);
    }

    lockSnowmen();
    redrawYard(hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();
}

void Snowmen::publishMqtt(bool reset)
{
    Json::Value root;
    Json::Value options;

    std::vector<SnowmanPicture *>::iterator ptr;
    int cnt = 0;
    for (ptr = availSnowman.begin(); ptr < availSnowman.end(); ptr++)
    {
        SnowmanPicture *pic = *(ptr);
        Json::Value obj;
        obj["id"] = cnt;
        obj["name"] = pic->name;
        options.append(obj);
        ++cnt;
    }

    root["current"] = who_right->name;
    root["available"] = options;
    root["reset"] = reset;

    Json::FastWriter fastWriter;
    std::string jsonMessage = fastWriter.write(root);
    mqtt->sendSnowmanJson(jsonMessage);
}

char * Snowmen::getMessage()
{
    return message2;
}

char * Snowmen::getMessageWho()
{
    return message_who;
}


