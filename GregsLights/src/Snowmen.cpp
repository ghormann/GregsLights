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
#include <sys/time.h>


#define SNOWBALL_DURATION 0.015
#define BALL_SIZE_1IN   3
#define BALL_SIZE_2IN   1.7
#define CANNON_BALL_SIZE_1IN 11.9
#define CANNON_BALL_SIZE_2IN 5.9

/*
 * Snowball flights are worked out in inches across the whole yard
 * (see SNOWMAN_SKY_RAISE) so they move at the same speed everywhere.
 */
#define YARD_RIGHT_X (SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*4 + SKY_GRID_WIDTH*2)
#define SNOWBALL_RADIUS 3.2     /* Inches */
#define CANNON_BALL_RADIUS 12.0
#define FLIGHT_STEP 2.0         /* Inches per frame */
#define FLIGHT_DELAY 0.01
#define RELEASE_Y 17            /* Where a throw leaves the hand */
#define OTHER_SIDE(pos) ((pos) == SNOWMAN_LEFT ? SNOWMAN_RIGHT : SNOWMAN_LEFT)


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
    who->name = "Red Among Us";
    who->rig = RIG_CREWMATE;
    who->armColor = new RGBColor(90, 5, 5);
    who->mouth_offset_y = 2;
    who->splash_offset_y = 2;
    who->splash_end_y = 17;
    who->nose_ball_dx = 1;
    who->name_offset_x = 6;
    who->show_misses = true;
    who->catchphrase = "Red looks sus...";
    who->signature = SIG_EJECT;
    who->signatureText = "Red was not The Impostor.";
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
    who->name = "Tails";
    who->rig = RIG_TAILS;
    who->armColor = new RGBColor(100, 45, 0);
    who->mouth_offset_y = 4;
    who->splash_offset_y = 4;
    who->splash_end_y = 18;
    who->nose_ball_dx = 1;
    who->name_offset_x = 30;
    who->show_misses = true;
    who->catchphrase = "Wait for me, Sonic!";
    who->signature = SIG_FLY;
    who->signatureText = "Look, I can fly!";
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
    who->name = "Minion";
    who->rig = RIG_MINION;
    who->armColor = new RGBColor(100, 80, 0);
    who->mouth_offset_y = 7;
    who->splash_offset_y = 7;
    who->splash_end_y = 19;
    who->nose_ball_dx = 3;
    who->name_offset_x = 24;
    who->show_misses = true;
    who->catchphrase = "Bello!";
    who->signature = SIG_BANANA;
    who->signatureText = "BANANA!";
    availSnowman.push_back(who);
    this->who_right = who;

    who = new SnowmanPicture();
    who->name = "Gingerbread Man";
    who->rig = RIG_GINGERBREAD;
    who->armColor = new RGBColor(65, 30, 8);
    who->mouth_offset_x = 7;
    who->mouth_offset_y = 1;
    who->splash_offset_y = 1;
    who->splash_end_y = 16;
    who->nose_ball_dx = 5;
    who->name_offset_x = 0;
    who->show_misses = true;
    who->catchphrase = "Run, run, as fast as you can!";
    who->signature = SIG_RUN_AWAY;
    who->signatureText = "You can't catch me, I'm the Gingerbread Man!";
    availSnowman.push_back(who);

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
    strcpy(message2, "Duck Snowman");
    int target = OTHER_SIDE(pos);
    double releaseX = (pos == SNOWMAN_LEFT ? 40 : YARD_RIGHT_X + 7);
    double endX = (target == SNOWMAN_RIGHT ? YARD_RIGHT_X + SNOWMEN_WIDTH + 6 : -6);

    // Ducks as it gets close, it sails over the top
    pos == SNOWMAN_LEFT ? throwLeft(true) : throwRight(true);
    ballFlight(pos, releaseX, RELEASE_Y, endX, 26, 2, SNOWBALL_RADIUS,
               pos == SNOWMAN_RIGHT ? throwBallColor : RGBColor::WHITE, FLIGHT_DELAY, REACT_DUCK);

    // Stand back up
    for (int d = 9; d >= 0; d--)
    {
        lockSnowmen();
        snowmen[target]->setBackground(RGBColor::BLACK);
        drawSnowmen(target, hatStatus[target], d);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION * 2);
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
    strcpy(message2, "Eat Snowball");
    int target = OTHER_SIDE(pos);
    double releaseX = (pos == SNOWMAN_LEFT ? 40 : YARD_RIGHT_X + 7);
    double mouthX, mouthY;
    if (target == SNOWMAN_RIGHT)
    {
        mouthX = YARD_RIGHT_X + SNOWMEN_WIDTH/2 - 8 + who_right->mouth_offset_x;
        mouthY = SNOWMEN_HEIGHT - 61 + who_right->mouth_offset_y;
    }
    else
    {
        mouthX = SNOWMEN_WIDTH/2 + 8;
        mouthY = SNOWMEN_HEIGHT - 61;
    }

    // Mouth opens as it arrives
    pos == SNOWMAN_LEFT ? throwLeft(true) : throwRight(true);
    ballFlight(pos, releaseX, RELEASE_Y, mouthX, mouthY, 6, SNOWBALL_RADIUS,
               pos == SNOWMAN_RIGHT ? throwBallColor : RGBColor::WHITE, FLIGHT_DELAY, REACT_EAT);

    // Chew and close
    for (int i = 9; i >= 0; i--)
    {
        lockSnowmen();
        snowmen[target]->setBackground(RGBColor::BLACK);
        drawSnowmen(target, hatStatus[target]);
        drawMouth(target, i * 0.3);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION * 2);
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

void Snowmen::hitNose(int snowmen)
{
    strcpy(message2, "Hit Nose");
    ++noseBalls[snowmen];

    lockSnowmen();
    getSnowmen(snowmen)->setBackground(RGBColor::BLACK);
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

    if (pos == SNOWMAN_RIGHT && who_right->pic == NULL && who_right->rig != RIG_SNOWMAN)
    {
        drawRig(who, offset, xShift, yShift);
    }
    else if ((pos == SNOWMAN_LEFT) || (who_right->pic == NULL))
    {
        xShift = yShift = 0;  // Only characters move

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
        x = (pos == SNOWMAN_RIGHT ? SNOWMEN_WIDTH - (SNOWMEN_WIDTH/2+14) + who_right->nose_ball_dx + xShift : SNOWMEN_WIDTH/2+12);
        who->drawCircle(x, SNOWMAN_NOSE_HEIGHT-1 + topOffset + yShift, BALL_SIZE_1IN, noseBallColor);
    }

    if (noseBalls[pos] > 1)
    {
        x = (pos == SNOWMAN_RIGHT ? SNOWMEN_WIDTH - (SNOWMEN_WIDTH/2+18) + who_right->nose_ball_dx + xShift : SNOWMEN_WIDTH/2+16);
        who->drawCircle(x, SNOWMAN_NOSE_HEIGHT-1 + topOffset + yShift, BALL_SIZE_1IN, noseBallColor);
    }

    // If fading then draw nose again.
    if (noseBallColor->getBlue() < 100 && (pos == SNOWMAN_LEFT || who_right->isPlainSnowman()))
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

/*
 * Wind up and release. ballFlight() carries it from there.
 */
void Snowmen::throwRight(bool loft)
{
    const int sholder_x = 28;
    const int sholder_y = 42;
    //const int ball_x[] = {12+30,12+30,11+30, 10+30, 9+30, 8+30, 8+30, 7+30, 7+30, 6+30, 33, 30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 1};
    const int ball_x[] = {SNOWMEN_WIDTH-12,SNOWMEN_WIDTH-12,SNOWMEN_WIDTH-11, SNOWMEN_WIDTH-10, SNOWMEN_WIDTH-9, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-7, SNOWMEN_WIDTH-7, SNOWMEN_WIDTH-6, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-10, SNOWMEN_WIDTH-12, SNOWMEN_WIDTH-14, SNOWMEN_WIDTH-16, SNOWMEN_WIDTH-19, SNOWMEN_WIDTH-22, SNOWMEN_WIDTH-25, SNOWMEN_WIDTH-28, SNOWMEN_WIDTH-32, SNOWMEN_WIDTH-36, SNOWMEN_WIDTH-40};
    const int ball_y[] = {16,16,16,16, 15, 15, 15, 14, 14, 13, 13, 12, 12, 11, 12, 12, 12, 12, 13, 14, 15, 17};
    const int elbow_x[] = {SNOWMEN_WIDTH-1,SNOWMEN_WIDTH-2,SNOWMEN_WIDTH-3, SNOWMEN_WIDTH-4, SNOWMEN_WIDTH-5, SNOWMEN_WIDTH-5, SNOWMEN_WIDTH-6, SNOWMEN_WIDTH-7, SNOWMEN_WIDTH-8, SNOWMEN_WIDTH-9};
    const int elbow_y[] = {30,30,30, 30, 30, 30, 30, 30, 30, 30};



    GenericGrid *right = getSnowmen(SNOWMAN_RIGHT);


    // Pull back the Arm
    for (int i = 0; i< 10; i++)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);

        right->plotLineWidth(elbow_x[i],elbow_y[i],sholder_x,sholder_y,2.0,who_right->armColor);
        right->plotLineWidth(elbow_x[i],elbow_y[i],ball_x[i],ball_y[i],2.0,who_right->armColor);
        right->drawCircle(ball_x[i],ball_y[i],BALL_SIZE_1IN, throwBallColor);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }

    // Move the Arm foward as a Straight line
    for (int i = 10; i< 22; i++)
    {
        lockSnowmen();
        right->setBackground(RGBColor::BLACK);
        drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);

        right->plotLineWidth(sholder_x,sholder_y,ball_x[i],ball_y[i],2.0,who_right->armColor);
        right->drawCircle(ball_x[i],ball_y[i],BALL_SIZE_1IN, throwBallColor);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }
}

/*
 * Wind up and release. ballFlight() carries it from there.
 */
void Snowmen::throwLeft(bool loft)
{
    const int sholder_x = 16;
    const int sholder_y = 42;
    const int ball_x[] = {12,12,11, 10, 9, 8, 8, 7, 7, 6, 8, 10, 12, 14, 16, 19, 22, 25, 28, 32, 36, 40};
    const int ball_y[] = {16,16,16,16, 15, 15, 15, 14, 14, 13, 13, 12, 12, 11, 12, 12, 12, 12, 13, 14, 15, 17};
    const int elbow_x[] = {1,2,3, 4, 5, 5, 6, 7, 8, 9};
    const int elbow_y[] = {30,30,30, 30, 30, 30, 30, 30, 30, 30};



    GenericGrid *left = getSnowmen(SNOWMAN_LEFT);

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
}

void Snowmen::throwLeftStickNose(bool arch)
{
    strcpy(message2, "Throw Left Nose");
    throwLeft(arch);
    ballFlight(SNOWMAN_LEFT, 40, RELEASE_Y,
               YARD_RIGHT_X + SNOWMEN_WIDTH - (SNOWMEN_WIDTH/2+14) + who_right->nose_ball_dx, SNOWMAN_NOSE_HEIGHT - 1,
               arch ? -6 : 16, SNOWBALL_RADIUS, RGBColor::WHITE, FLIGHT_DELAY);
    hitNose(SNOWMAN_RIGHT);
}

void Snowmen::throwRightStickNose(bool arch)
{
    strcpy(message2, "Throw Right Stick Nose");
    throwRight(arch);
    ballFlight(SNOWMAN_RIGHT, YARD_RIGHT_X + 7, RELEASE_Y, SNOWMEN_WIDTH/2 + 12, SNOWMAN_NOSE_HEIGHT - 1,
               arch ? -6 : 16, SNOWBALL_RADIUS, throwBallColor, FLIGHT_DELAY);
    hitNose(SNOWMAN_LEFT);
}

void Snowmen::throwHitHat(int pos)
{
    strcpy(message2, "Throw Hit Hat");
    if (pos == SNOWMAN_LEFT)
    {
        throwLeft(true);
        ballFlight(SNOWMAN_LEFT, 40, RELEASE_Y, YARD_RIGHT_X + 12, 13, -6, SNOWBALL_RADIUS, RGBColor::WHITE, FLIGHT_DELAY);
        hitHat(SNOWMAN_RIGHT);
    }
    else
    {
        throwRight(true);
        ballFlight(SNOWMAN_RIGHT, YARD_RIGHT_X + 7, RELEASE_Y, SNOWMEN_WIDTH - 12, 13, -6, SNOWBALL_RADIUS, throwBallColor, FLIGHT_DELAY);
        hitHat(SNOWMAN_LEFT);
    }
}

void Snowmen::throwGround(int pos, bool goHigh)
{
    int target = OTHER_SIDE(pos);
    // Lands on the snow pile next to the other snowman
    double groundX = (target == SNOWMAN_RIGHT ? YARD_RIGHT_X - 4 : SNOWMEN_WIDTH + 2);
    if (pos == SNOWMAN_LEFT)
    {
        throwLeft(goHigh);
        ballFlight(pos, 40, RELEASE_Y, groundX, SNOWMEN_HEIGHT - 3, goHigh ? -8 : 12, SNOWBALL_RADIUS, RGBColor::WHITE, FLIGHT_DELAY);
    }
    else
    {
        throwRight(goHigh);
        ballFlight(pos, YARD_RIGHT_X + 7, RELEASE_Y, groundX, SNOWMEN_HEIGHT - 3, goHigh ? -8 : 12, SNOWBALL_RADIUS, throwBallColor, FLIGHT_DELAY);
    }

    ++groundSnowLevel[target];
    lockSnowmen();
    drawGroundSnow(target, RGBColor::WHITE);
    releaseSnowmen();
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
    int y, snowman_x = 0;
    double size_1in = CANNON_BALL_SIZE_1IN;
    // Muzzle and impact in yard inches
    double muzzleX, muzzleY, impactX, impactY = 35;

    strcpy(message2, "Cannon Shot");

    if (snowmen_pos == SNOWMAN_LEFT)
    {
        cannon = RGBPicture::getPicture("cannon_right.png");
        muzzleX = SNOWMEN_WIDTH + 19*2;
        muzzleY = (20 + 3) * 2;
        impactX = YARD_RIGHT_X + 12;
    }
    else
    {
        cannon = RGBPicture::getPicture("cannon_left.png");
        muzzleX = YARD_RIGHT_X - SPLASH_GRID_WIDTH*2 + 3*2;
        muzzleY = (20 + 2) * 2;
        impactX = SNOWMEN_WIDTH - 12;
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
     * Fire: one arc up through the sky, same speed the whole way
     */
    cannonOnSplash[snowmen_pos] = cannon;
    ballFlight(-1, muzzleX, muzzleY, impactX, impactY, -4, CANNON_BALL_RADIUS, RGBColor::WHITE, SNOWBALL_DURATION);

    /*
     * Explosion
     */
    for (int i = 0; i < 22; i++)
    {
        lockSnowmen();
        drawBallInYard(impactX, impactY + i, CANNON_BALL_RADIUS + i * 2.0, RGBColor::WHITE);
        releaseSnowmen();
        write_data(SNOWBALL_DURATION);
    }
    size_1in = CANNON_BALL_SIZE_1IN + 44;
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
    cannonOnSplash[snowmen_pos] = NULL;
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
        if (!who_right->isPlainSnowman())
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

    if (!who_right->isPlainSnowman())
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
    if (who_right->signature != SIG_NONE)
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
    case SIG_RUN_AWAY:
        characterRunAway();
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
 * Where each grid sits in the yard, in inches
 */
void Snowmen::getYardAreas(YardArea areas[5])
{
    areas[0] = {snowmen[SNOWMAN_LEFT], 0, 0, 1, SNOWMAN_LEFT, true};
    areas[1] = {splash[SNOWMAN_LEFT], SNOWMEN_WIDTH, 0, 2, SNOWMAN_LEFT, false};
    areas[2] = {skyGrid, SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*2, -SNOWMAN_SKY_RAISE, 2, SNOWMAN_LEFT, false};
    areas[3] = {splash[SNOWMAN_RIGHT], SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*2 + SKY_GRID_WIDTH*2, 0, 2, SNOWMAN_RIGHT, false};
    areas[4] = {snowmen[SNOWMAN_RIGHT], YARD_RIGHT_X, 0, 1, SNOWMAN_RIGHT, true};
}

/*
 * A ball at a yard position, drawn on every grid it overlaps at the
 * right size for that grid's spacing
 */
void Snowmen::drawBallInYard(double x, double y, double radius, RGBColor *color)
{
    YardArea areas[5];
    getYardAreas(areas);
    for (auto &area : areas)
    {
        double gx = (x - area.x0) / area.density;
        double gy = (y - area.y0) / area.density;
        double r = radius / area.density;
        if (gx + r < 0 || gy + r < 0 || gx - r >= area.grid->getGridWidth() || gy - r >= area.grid->getGridHeight())
            continue;
        area.grid->drawCircle((int)round(gx), (int)round(gy), r, color);
    }
}

/*
 * Puts back whatever a ball at this position was covering
 */
void Snowmen::eraseBallInYard(double x, double y, double radius)
{
    YardArea areas[5];
    getYardAreas(areas);
    for (auto &area : areas)
    {
        double gx = (x - area.x0) / area.density;
        double gy = (y - area.y0) / area.density;
        double r = radius / area.density + 1;
        if (gx + r < 0 || gy + r < 0 || gx - r >= area.grid->getGridWidth() || gy - r >= area.grid->getGridHeight())
            continue;
        if (area.isSnowman)
        {
            area.grid->setBackground(RGBColor::BLACK);
            drawSnowmen(area.pos, hatStatus[area.pos]);
        }
        else
        {
            area.grid->drawCircle((int)round(gx), (int)round(gy), r, RGBColor::BLACK);
            if (area.grid == splash[area.pos])
            {
                if (cannonOnSplash[area.pos] != NULL)
                    area.grid->showPictureNow(*cannonOnSplash[area.pos], 0, 20, true);
                drawGroundSnow(area.pos, RGBColor::WHITE);
            }
        }
    }
}

/*
 * The throwing arm swinging down after the ball leaves the hand
 */
void Snowmen::armFollowThrough(int pos, int frame)
{
    const int hand_x[] = {41, 42, 43, 44, 43, 42};
    const int hand_y[] = {20, 26, 32, 38, 44, 50};
    GenericGrid *grid = snowmen[pos];
    grid->setBackground(RGBColor::BLACK);
    drawSnowmen(pos, hatStatus[pos]);
    if (frame < 6)
    {
        if (pos == SNOWMAN_LEFT)
            grid->plotLineWidth(16, 42, hand_x[frame], hand_y[frame], 2.0, RGBColor::WHITE);
        else
            grid->plotLineWidth(28, 42, SNOWMEN_WIDTH - hand_x[frame], hand_y[frame], 2.0, who_right->armColor);
    }
}

/*
 * One flight from (x0,y0) to (x1,y1) in yard inches, peaking at apexY
 * over the middle of the sky. Lights only exist inside the grids, and
 * the sky is the only grid between the splash grids, so the ball has to
 * cross each splash/sky edge where both have lights (between r and
 * 32-r inches down) or part of it disappears. The path is a smooth
 * spline through those crossings and it moves the same distance every
 * frame whichever grid it is on. thrower (or -1) gets its follow
 * through; the other side can duck or open its mouth.
 */
void Snowmen::ballFlight(int thrower, double x0, double y0, double x1, double y1, double apexY,
                         double radius, RGBColor *color, double delay, FlightReaction react)
{
    int target = (thrower < 0 ? (x1 > x0 ? SNOWMAN_RIGHT : SNOWMAN_LEFT) : OTHER_SIDE(thrower));
    const double skyLeft = SNOWMEN_WIDTH + SPLASH_GRID_WIDTH*2;
    const double skyRight = skyLeft + SKY_GRID_WIDTH*2;
    const double skyBottom = SKY_GRID_HEIGHT*2 - SNOWMAN_SKY_RAISE;
    double lowest = skyBottom - radius - 1;     // Visible band at the edges
    double highest = radius + 1;
    double topOfSky = -SNOWMAN_SKY_RAISE + radius;
    if (apexY < topOfSky)
        apexY = topOfSky;

    // Waypoints: start, both sky edges, middle of the sky, end
    std::vector<std::pair<double, double>> way;
    way.push_back({x0, y0});
    double edgeIn = (x1 > x0 ? skyLeft : skyRight);
    double edgeOut = (x1 > x0 ? skyRight : skyLeft);
    bool crossesSky = (fmin(x0, x1) < skyLeft && fmax(x0, x1) > skyRight);
    if (crossesSky)
    {
        double yIn = fmax(highest, fmin(lowest, (y0 + apexY) / 2));
        double yOut = fmax(highest, fmin(lowest, (y1 + apexY) / 2));
        way.push_back({edgeIn, yIn});
        way.push_back({(skyLeft + skyRight) / 2, apexY});
        way.push_back({edgeOut, yOut});
    }
    way.push_back({x1, y1});

    // Catmull-Rom spline through the waypoints, sampled finely
    std::vector<std::pair<double, double>> path;
    int n = way.size();
    for (int i = 0; i < n - 1; i++)
    {
        auto p0 = way[i > 0 ? i - 1 : 0], p1 = way[i], p2 = way[i + 1], p3 = way[i + 2 < n ? i + 2 : n - 1];
        for (int k = 0; k < 60; k++)
        {
            double t = k / 60.0, t2 = t * t, t3 = t2 * t;
            double x = 0.5 * (2*p1.first + (-p0.first + p2.first)*t + (2*p0.first - 5*p1.first + 4*p2.first - p3.first)*t2
                              + (-p0.first + 3*p1.first - 3*p2.first + p3.first)*t3);
            double y = 0.5 * (2*p1.second + (-p0.second + p2.second)*t + (2*p0.second - 5*p1.second + 4*p2.second - p3.second)*t2
                              + (-p0.second + 3*p1.second - 3*p2.second + p3.second)*t3);
            path.push_back({x, y});
        }
    }
    path.push_back(way[n - 1]);

    // Walk it at a constant speed
    std::vector<std::pair<double, double>> frames;
    frames.push_back(path[0]);
    double carried = 0;
    for (size_t i = 1; i < path.size(); i++)
    {
        double seg = hypot(path[i].first - path[i-1].first, path[i].second - path[i-1].second);
        carried += seg;
        while (carried >= FLIGHT_STEP)
        {
            carried -= FLIGHT_STEP;
            double back = (seg > 0 ? carried / seg : 0);
            frames.push_back({path[i].first - (path[i].first - path[i-1].first) * back,
                              path[i].second - (path[i].second - path[i-1].second) * back});
        }
    }
    frames.push_back(path.back());

    int count = frames.size();
    double prevX = 0, prevY = 0;
    for (int i = 0; i < count; i++)
    {
        double t = (double)i / (count - 1);
        double x = frames[i].first;
        double y = frames[i].second;

        lockSnowmen();
        if (i > 0)
        {
            eraseBallInYard(prevX, prevY, radius);
        }
        if (thrower >= 0 && i <= 6)
        {
            armFollowThrough(thrower, i);
        }
        if (react == REACT_DUCK)
        {
            int duck = (t < 0.5 ? 0 : (int)fmin(9, (t - 0.5) * 30));
            snowmen[target]->setBackground(RGBColor::BLACK);
            drawSnowmen(target, hatStatus[target], duck);
        }
        else if (react == REACT_EAT && t > 0.75)
        {
            snowmen[target]->setBackground(RGBColor::BLACK);
            drawSnowmen(target, hatStatus[target]);
            drawMouth(target, fmin(2.7, (t - 0.75) * 12));
        }
        // Eaten balls disappear into the mouth
        if (!(react == REACT_EAT && i == count - 1))
        {
            drawBallInYard(x, y, radius, color);
        }
        releaseSnowmen();
        write_data(delay);
        prevX = x;
        prevY = y;
    }

    lockSnowmen();
    eraseBallInYard(prevX, prevY, radius);
    if (react == REACT_DUCK)
    {
        snowmen[target]->setBackground(RGBColor::BLACK);
        drawSnowmen(target, hatStatus[target], 9);
    }
    else if (react == REACT_EAT)
    {
        drawMouth(target, 2.7);
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
    YardArea areas[5];
    getYardAreas(areas);
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
    flying = true;
    skyText(who_right->signatureText, RGBColor::YELLOW);
    slideCharacter(0, 0, 0, -SNOWMEN_HEIGHT, 25, 0.03);
    write_data(1.0);
    slideCharacter(0, -SNOWMEN_HEIGHT, 0, 0, 30, 0.05);
    // Little bounce on landing
    slideCharacter(0, 0, 0, -4, 2, 0.08);
    slideCharacter(0, -4, 0, 0, 2, 0.08);
    flying = false;
    lockSnowmen();
    getSnowmen(SNOWMAN_RIGHT)->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
    releaseSnowmen();
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
    GenericGrid *left = getSnowmen(SNOWMAN_LEFT);

    int noseX = SNOWMEN_WIDTH/2 + 12;
    int noseY = SNOWMAN_NOSE_HEIGHT - 1;
    if (who_right->pic == NULL)
    {
        // Drawn characters wind up and throw it
        throwBallColor = RGBColor::YELLOW;
        throwRight(true);
        ballFlight(SNOWMAN_RIGHT, YARD_RIGHT_X + 7, RELEASE_Y, noseX, noseY, -6, SNOWBALL_RADIUS, RGBColor::YELLOW, FLIGHT_DELAY);
        throwBallColor = RGBColor::WHITE;
    }
    else
    {
        // Pictures have no arm: it comes out of his hands
        ballFlight(-1, YARD_RIGHT_X + 12, 50, noseX, noseY, -6, SNOWBALL_RADIUS, RGBColor::YELLOW, FLIGHT_DELAY);
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

/*
 * Seconds, for animation that keeps moving between frames (tails, legs)
 */
static double animationClock()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec % 10000 + tv.tv_usec / 1000000.0;
}

/*
 * Filled box, clipped to the grid
 */
void Snowmen::fillBox(GenericGrid *g, int x0, int y0, int x1, int y1, RGBColor *color)
{
    for (int x = (x0 < 0 ? 0 : x0); x < x1 && x < g->getGridWidth(); x++)
    {
        for (int y = (y0 < 0 ? 0 : y0); y < y1 && y < g->getGridHeight(); y++)
        {
            g->getPixal(x, y)->set(color);
        }
    }
}

/*
 * Draws the right character in code. Like the snowman, ducking squashes
 * it: the head drops 3x the offset, the middle 2x and the bottom 1x.
 */
void Snowmen::drawRig(GenericGrid *g, int offset, int dx, int dy)
{
    int head = 3 * offset + dy;
    int mid = 2 * offset + dy;
    int low = offset + dy;
    switch (who_right->rig)
    {
    case RIG_MINION:
        drawMinion(g, head, mid, low, dx, dy);
        break;
    case RIG_CREWMATE:
        drawCrewmate(g, head, mid, low, dx, dy);
        break;
    case RIG_TAILS:
        drawTails(g, head, mid, low, dx, dy);
        break;
    case RIG_GINGERBREAD:
        drawGingerbread(g, head, mid, low, dx, dy);
        break;
    default:
        break;
    }
}

void Snowmen::drawMinion(GenericGrid *g, int head, int mid, int low, int dx, int dy)
{
    static RGBColor *yellow = new RGBColor(100, 80, 0);
    static RGBColor *denim = new RGBColor(10, 30, 90);
    static RGBColor *dark = new RGBColor(8, 8, 8);
    static RGBColor *metal = new RGBColor(45, 45, 50);
    static RGBColor *iris = new RGBColor(45, 22, 5);

    // Capsule body
    g->drawCircle(24+dx, 32+head, 13, yellow);
    fillBox(g, 11+dx, 32+head, 38+dx, 66+mid, yellow);

    // Overalls: bib, straps and round bottom
    fillBox(g, 11+dx, 62+mid, 38+dx, 74+low, denim);
    g->drawCircle(24+dx, 72+low, 13, denim);
    fillBox(g, 17+dx, 53+mid, 32+dx, 63+mid, denim);
    g->plotLineWidth(17+dx, 54+mid, 12+dx, 47+head, 1.5, denim);
    g->plotLineWidth(31+dx, 54+mid, 37+dx, 47+head, 1.5, denim);

    // Legs and shoes
    fillBox(g, 16+dx, 84+low, 22+dx, 89+low, denim);
    fillBox(g, 27+dx, 84+low, 33+dx, 89+low, denim);
    fillBox(g, 12+dx, 89+low, 23+dx, 92+low, dark);
    fillBox(g, 26+dx, 89+low, 35+dx, 92+low, dark);

    // Resting arm with a black glove
    g->plotLineWidth(12+dx, 50+mid, 7+dx, 61+mid, 2.5, yellow);
    g->drawCircle(7+dx, 62+mid, 2, dark);

    // Goggle strap and one big goggle facing left
    fillBox(g, 11+dx, 27+head, 38+dx, 32+head, dark);
    g->drawCircle(17+dx, 29+head, 6, metal);
    g->drawCircle(17+dx, 29+head, 4.5, RGBColor::WHITE);
    g->drawCircle(15+dx, 29+head, 2, iris);
    g->drawCircle(14+dx, 29+head, 0.8, dark);

    // A few hairs (the hat covers them when it is on)
    g->plotLineWidth(22+dx, 19+head, 20+dx, 14+head, 1, dark);
    g->plotLineWidth(25+dx, 19+head, 26+dx, 13+head, 1, dark);
    g->plotLineWidth(28+dx, 20+head, 31+dx, 15+head, 1, dark);

    // Smile
    g->plotLineWidth(13+dx, 40+head, 18+dx, 42+head, 1, dark);
}

void Snowmen::drawCrewmate(GenericGrid *g, int head, int mid, int low, int dx, int dy)
{
    static RGBColor *red = new RGBColor(90, 5, 5);
    static RGBColor *shade = new RGBColor(45, 0, 0);
    static RGBColor *visor = new RGBColor(40, 75, 95);

    // Backpack behind (to the right, since it faces left)
    fillBox(g, 37+dx, 38+head, 45+dx, 66+mid, shade);

    // Bean shaped body
    g->drawCircle(24+dx, 32+head, 14, red);
    fillBox(g, 10+dx, 32+head, 39+dx, 74+mid, red);

    // Legs
    fillBox(g, 11+dx, 72+mid, 22+dx, 86+low, red);
    fillBox(g, 27+dx, 72+mid, 38+dx, 86+low, red);
    g->drawCircle(16+dx, 86+low, 5, red);
    g->drawCircle(32+dx, 86+low, 5, red);

    // Visor with a highlight
    g->drawCircle(14+dx, 31+head, 5, visor);
    g->drawCircle(22+dx, 31+head, 5, visor);
    fillBox(g, 14+dx, 26+head, 23+dx, 37+head, visor);
    fillBox(g, 11+dx, 28+head, 17+dx, 30+head, RGBColor::WHITE);
}

void Snowmen::drawTails(GenericGrid *g, int head, int mid, int low, int dx, int dy)
{
    static RGBColor *orange = new RGBColor(100, 45, 0);
    static RGBColor *eye = new RGBColor(10, 45, 100);
    static RGBColor *dark = new RGBColor(8, 8, 8);
    static RGBColor *shoe = new RGBColor(90, 5, 5);
    double t = animationClock();

    // Two tails behind him, or a spinning propeller when flying. They must
    // land in the same place on every redraw unless flying: some moves
    // redraw without clearing first.
    int pivotX = 33 + dx, pivotY = 58 + mid;
    for (int k = 0; k < 2; k++)
    {
        double a;
        if (flying)
            a = t * 25 + k * M_PI;
        else
            a = -0.6 + k * 0.9;
        int endX = pivotX + (int)(15 * cos(a));
        int endY = pivotY + (int)(15 * sin(a) * (flying ? 0.5 : 1.0));
        g->plotLineWidth(pivotX, pivotY, endX, endY, 5, orange);
        g->drawCircle(endX, endY, 3, RGBColor::WHITE);
    }

    // Legs and red shoes
    g->plotLineWidth(22+dx, 60+mid, 20+dx, 80+low, 3, orange);
    g->plotLineWidth(28+dx, 60+mid, 30+dx, 80+low, 3, orange);
    fillBox(g, 13+dx, 80+low, 24+dx, 86+low, shoe);
    fillBox(g, 26+dx, 80+low, 37+dx, 86+low, shoe);
    fillBox(g, 16+dx, 82+low, 21+dx, 84+low, RGBColor::WHITE);
    fillBox(g, 29+dx, 82+low, 34+dx, 84+low, RGBColor::WHITE);

    // Body with white chest, resting arm with a glove
    g->drawCircle(25+dx, 51+mid, 9, orange);
    g->drawCircle(22+dx, 53+mid, 6, RGBColor::WHITE);
    g->plotLineWidth(19+dx, 46+mid, 14+dx, 59+mid, 2.5, orange);
    g->drawCircle(14+dx, 60+mid, 2.5, RGBColor::WHITE);

    // Head, ears and forehead tufts
    g->plotLineWidth(20+dx, 22+head, 22+dx, 11+head, 4, orange);
    g->plotLineWidth(22+dx, 11+head, 26+dx, 21+head, 4, orange);
    g->plotLineWidth(28+dx, 21+head, 31+dx, 10+head, 4, orange);
    g->plotLineWidth(31+dx, 10+head, 34+dx, 22+head, 4, orange);
    g->drawCircle(25+dx, 30+head, 11, orange);
    g->plotLineWidth(16+dx, 23+head, 10+dx, 25+head, 2, orange);
    g->plotLineWidth(16+dx, 26+head, 10+dx, 29+head, 2, orange);

    // White muzzle, black nose, big blue eye
    g->drawCircle(16+dx, 35+head, 5, RGBColor::WHITE);
    g->drawCircle(20+dx, 37+head, 4, RGBColor::WHITE);
    g->drawCircle(11+dx, 33+head, 1.5, dark);
    g->drawCircle(20+dx, 27+head, 3.5, RGBColor::WHITE);
    g->drawCircle(18+dx, 27+head, 1.5, eye);
}

void Snowmen::drawGingerbread(GenericGrid *g, int head, int mid, int low, int dx, int dy)
{
    static RGBColor *cookie = new RGBColor(65, 30, 8);
    static RGBColor *gumRed = new RGBColor(100, 0, 0);
    static RGBColor *gumGreen = new RGBColor(0, 100, 0);
    static RGBColor *dark = new RGBColor(8, 8, 8);

    // Legs and arms swing while running
    int swing = running ? (int)(6 * sin(animationClock() * 18)) : 0;
    int lfX = 15 - swing, rfX = 33 + swing;
    g->plotLineWidth(20+dx, 66+mid, lfX+dx, 87+low, 7, cookie);
    g->plotLineWidth(28+dx, 66+mid, rfX+dx, 87+low, 7, cookie);
    g->plotLineWidth(17+dx, 48+mid, 6+dx, 60+mid + swing, 6, cookie);
    g->plotLineWidth(31+dx, 48+mid, 42+dx, 60+mid - swing, 6, cookie);

    g->drawCircle(24+dx, 56+mid, 12, cookie);
    fillBox(g, 20+dx, 36+head, 29+dx, 46+mid, cookie);  // Neck
    g->drawCircle(24+dx, 30+head, 10, cookie);

    // Icing squiggles on the wrists and ankles
    int ends[][2] = {{6, 57 + swing}, {42, 57 - swing}, {lfX, 83}, {rfX, 83}};
    for (int i = 0; i < 4; i++)
    {
        int x = ends[i][0] + dx;
        int y = ends[i][1] + (i < 2 ? mid : low);
        g->plotLineWidth(x-3, y, x-1, y-2, 1, RGBColor::WHITE);
        g->plotLineWidth(x-1, y-2, x+1, y, 1, RGBColor::WHITE);
        g->plotLineWidth(x+1, y, x+3, y-2, 1, RGBColor::WHITE);
    }

    // Gumdrop buttons
    g->drawCircle(24+dx, 49+mid, 1.5, gumRed);
    g->drawCircle(24+dx, 56+mid, 1.5, gumGreen);
    g->drawCircle(24+dx, 63+mid, 1.5, gumRed);

    // Face: icing eyes and smile
    g->drawCircle(20+dx, 27+head, 1.5, RGBColor::WHITE);
    g->drawCircle(27+dx, 27+head, 1.5, RGBColor::WHITE);
    g->drawCircle(20+dx, 27+head, 0.5, dark);
    g->drawCircle(27+dx, 27+head, 0.5, dark);
    g->plotLineWidth(19+dx, 34+head, 22+dx, 36+head, 1, RGBColor::WHITE);
    g->plotLineWidth(22+dx, 36+head, 26+dx, 34+head, 1, RGBColor::WHITE);
}

/*
 * Gingerbread Man runs off with his legs going, taunts, and runs back
 */
void Snowmen::characterRunAway()
{
    strcpy(message2, "Run Away");
    running = true;
    slideCharacter(0, 0, SNOWMEN_WIDTH + 10, 0, 18, 0.05);
    skyText(who_right->signatureText, RGBColor::YELLOW);
    write_data(2.5);
    getSkyGrid()->setBackground(RGBColor::BLACK);
    slideCharacter(SNOWMEN_WIDTH + 10, 0, 0, 0, 18, 0.05);
    running = false;
    lockSnowmen();
    getSnowmen(SNOWMAN_RIGHT)->setBackground(RGBColor::BLACK);
    drawSnowmen(SNOWMAN_RIGHT, hatStatus[SNOWMAN_RIGHT]);
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


