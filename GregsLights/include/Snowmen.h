#ifndef SNOWMEN_H
#define SNOWMEN_H

#include "pthread.h"
#include "controller/Bulb.h"
#include "controller/E131Network.h"
#include "TimeInfo.h"
#include "GenericGrid.h"
#include "GregMQTT.h"
#include "RGBPicture.h"

#define SNOWMEN_HEIGHT 95
#define SNOWMEN_WIDTH 47

#define SPLASH_GRID_HEIGHT 48
#define SPLASH_GRID_WIDTH 24

#define SKY_GRID_HEIGHT 24
#define SKY_GRID_WIDTH 96

#define SNOWMAN_RIGHT 1
#define SNOWMAN_LEFT  0

/* Throws before the right snowman is replaced. Shorter when someone is waiting on a vote */
#define SNOWMAN_STEPS 40
#define SNOWMAN_STEPS_VOTE_WAITING 10
#define SNOWMAN_MIN_MOVES 4  /* Before a random cannon shot can replace it */
#define SNOWMAN_SIGNATURE_BY_MOVE 2  /* Force the signature move if it hasn't happened by now */
#define SNOWMAN_SIGNATURE_CHANCE 6   /* Otherwise 1 in N moves is the signature move */

/*
 * Each character's special move. Set in createSnowmanPictures().
 */
enum SignatureMove
{
    SIG_NONE,
    SIG_STEAL_HAT,      /* Grinch: steals the left snowman's hat and keeps it */
    SIG_FALL,           /* Slow motion fall off the bottom */
    SIG_FLY,            /* Flies up out of sight and floats back down */
    SIG_DASH,           /* Zooms off and runs around the yard */
    SIG_EJECT,          /* Floats away into space */
    SIG_BANANA,         /* Throws a yellow snowball at the left snowman */
    SIG_EAT_EVERYTHING, /* Eats every snowball thrown at him */
    SIG_TWINKLE_LIGHTS, /* Plugs in 25,000 twinkle lights on the left snowman, garage too */
    SIG_RUN_AWAY        /* Runs off and back: "You can't catch me!" */
};

/*
 * Characters drawn in code instead of from a picture. They follow the
 * snowman's layout (head at y 20-40 facing left, shoulder at 28,42) so
 * they can do every move, including throwing back.
 */
enum CharacterRig
{
    RIG_SNOWMAN,
    RIG_MINION,
    RIG_CREWMATE,
    RIG_TAILS,
    RIG_GINGERBREAD
};

/* What the target does while a snowball flies at it */
enum FlightReaction
{
    REACT_NONE,
    REACT_DUCK,
    REACT_EAT
};

struct YardArea
{
    GenericGrid *grid;
    int x0;         /* Top left corner in inches */
    int y0;
    int density;    /* Inches between lights */
    int pos;        /* SNOWMAN_LEFT/RIGHT side, for redrawing */
    bool isSnowman;
};

class GarageSign;

/*
 * Physical layout in inches, left to right: left snowman, left splash,
 * sky, right splash, right snowman. Snowman grids have 1" spacing,
 * splash and sky have 2". The sky hangs this much higher than the rest.
 */
#define SNOWMAN_SKY_RAISE 16

#define SNOWMAN_NOSE_HEIGHT (SNOWMEN_HEIGHT-65)

/* animationSpeed > 1 speeds up everything while a fast character (Sonic) is up */
#define write_data(pause)  usleep((pause) * 1000000 / animationSpeed)

class SnowmanPicture
{
    public:
    RGBPicture *pic = NULL;
    int pic_offset_x = 0;
    int pic_offset_y = 0;
    std::string name = "Snowman";
    int mouth_offset_x = 0;
    int mouth_offset_y = 0;
    int splash_end_y = 0;
    int splash_offset_y = 0;
    int name_offset_x = 0;
    double mouth_multiplier = 1;
    bool show_misses = true;
    std::string catchphrase = "";       /* On the sky when it arrives */
    SignatureMove signature = SIG_NONE;
    std::string signatureText = "";     /* On the sky during the signature move */
    double speed = 1.0;                 /* > 1 speeds up all animation while it is up */
    CharacterRig rig = RIG_SNOWMAN;     /* How to draw it when there is no picture */
    RGBColor *armColor = RGBColor::WHITE;
    int nose_ball_dx = 0;               /* Moves stuck snowballs onto its face */
    bool isPlainSnowman() { return pic == NULL && rig == RIG_SNOWMAN; }
};

class SnowmenGrid: public GenericGrid
{
public:
    SnowmenGrid(int width_, int height_, int dummy_with_, int dummy_height);
    virtual RGBLight * getPixal(int x, int y);  /* Overriden */
    void setPixal(int x, int y, RGBLight * p);
protected:
    virtual RGBLight * getBoard(int x, int y);
private:
    int getPos(int x, int y);
    RGBLight *pixals[SNOWMEN_HEIGHT * SNOWMEN_WIDTH];
    RGBLight *board[SNOWMEN_HEIGHT * SNOWMEN_WIDTH];

};


class Snowmen
{
public:
    Snowmen(bool skipTime, E131Network * network[], GregMQTT *mqtt);
    virtual ~Snowmen();
    void test_snowmen(void);
    void run();
    void setBulb(int i, Bulb *b);
    char * getMessage();
    char * getMessageWho();
    SnowmenGrid *getSnowmen(int pos);
    SnowmenGrid *getSplashGrid(int pos);
    SnowmenGrid *getSkyGrid();
    void lockSnowmen();
    void releaseSnowmen();
    pthread_mutex_t* getLock();
    void setGarageSign(GarageSign *g);
protected:
    void throwLeftStickNose(bool arch);
    void throwRightStickNose(bool arch);
    void throwHitHat(int pos);
    void throwGround(int pos, bool goHigh);
    void cannonShot(int snowmen_pos);
    void fadeSnow();
    void fadeNoseBalls();
    void drawGroundSnow(int pos, RGBColor* color);
    /*
        void hit_low_right(double pause);
        void hit_low_left(double pause);
        void hit_high_left(double pause);
        void hit_high_right(double pause);
        void miss_going_left(double pause);
        void miss_going_right(double pause);
        void catch_going_left(double pause, double pause2);
        void catch_going_right(double pause, double pause2);
        void set_snowmen(int side, int,int,int,int,int,int,int);
        */
    void do_it_snowmen();
private:
    void drawSnowmen(int pos, bool withHat = true, int offset=0, int xShift=0, int yShift=0);
    void placeHatBack(int pos);
    void throwLeft(bool loft);
    void throwRight(bool loft);
    void hitNose(int snowmen);
    void hitHat(int pos);
    void duckSnowman(int pos);
    void eatSnowball(int pos);
    void drawMouth(int pos, double mouthPos);
    void updateSnowmanFromMQTT();
    void setAllOff();
    void resetSnowmen(int pos);
    void publishMqtt(bool reset = false);
    RGBColor *noseBallColor;
    pthread_mutex_t lock;
    double groundSnowLevel[2];
    int noseBalls[2];
    bool hatStatus[2];
    SnowmenGrid *snowmen[2];
    SnowmenGrid *splash[2];
    SnowmenGrid *skyGrid;
    // Signature moves
    void signatureMove();
    void skyText(const std::string &text, RGBColor *color);
    void slideCharacter(int fromX, int fromY, int toX, int toY, int frames, double delay, RGBColor *trail = NULL);
    void flyAcross(GenericGrid *grid, int pos, RGBPicture *pic, RGBColor *streak, int fromX, int toX, int y, int step);
    void drawPictureInYard(RGBPicture *pic, int x, int y);
    void getYardAreas(YardArea areas[5]);
    void drawBallInYard(double x, double y, double radius, RGBColor *color);
    void eraseBallInYard(double x, double y, double radius);
    void ballFlight(int thrower, double x0, double y0, double x1, double y1, double apexY,
                    double radius, RGBColor *color, double delay, FlightReaction react = REACT_NONE);
    void armFollowThrough(int pos, int frame);
    void drawCannon(int pos, int dropY, int recoil, int flash = 0);
    bool cannonUp[2] = {false, false};  /* Redrawn when a ball passes over it */
    void redrawYard(bool withRightHat);
    void stealHat();
    void characterFall();
    void characterFly();
    void characterDash();
    void characterEject();
    void bananaThrow();
    void twinkleLights();
    void useTestCharacter();
    void characterRunAway();
    // Drawn characters
    void drawRig(GenericGrid *g, int offset, int dx, int dy);
    void drawMinion(GenericGrid *g, int head, int mid, int low, int dx, int dy);
    void drawCrewmate(GenericGrid *g, int head, int mid, int low, int dx, int dy);
    void drawTails(GenericGrid *g, int head, int mid, int low, int dx, int dy);
    void drawGingerbread(GenericGrid *g, int head, int mid, int low, int dx, int dy);
    void fillBox(GenericGrid *g, int x0, int y0, int x1, int y1, RGBColor *color);
    RGBColor *throwBallColor = RGBColor::WHITE;  /* Right side's snowball */
    bool flying = false;    /* Tails spins his tails */
    bool running = false;   /* Legs swing */
    double animationSpeed = 1.0;
    bool hatStolen = false;       /* Right character is wearing the left snowman's hat */
    int signatureCount = 0;       /* Signature moves by the current character */
    SnowmanPicture *testCharacter = NULL;
    GarageSign *garageSign = NULL;
    char message2[100];
    int snowmanStepCount = 0;
    int nonSpecialSnowmanCount = 0;
    char message_who[30];
    TimeInfo *timeinfo;
    SnowmanPicture *who_right;
    GregMQTT *mqtt;
    void createSnowmanPictures();
    std::vector<SnowmanPicture*> availSnowman;
};

#endif // SNOWMEN_H
