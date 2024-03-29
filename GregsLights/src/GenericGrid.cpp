#include "GenericGrid.h"
#include "stdio.h"
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <assert.h>
#include <Magick++.h>

using namespace std;


GenericGrid::GenericGrid(int width_, int height_, int dummy_width_, int dummy_height_)
{
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        throw "Mutext init failed for Generic Grid";
    }

    //ctor
    sprintf(message, "Booting up: %s", (TimeInfo::getInstance()->isNewYears() ? "for new Years" : "for Christmas"));
    this->gridWidth = width_;
    this->gridHeight = height_;
    this->dummy_height = dummy_height_;
    this->dummy_width = dummy_width_;
    this->timeInfo = TimeInfo::getInstance();
    this->interrupt = false;
    currentX = 0;
    currentY = 0;
    interruptAble = true;
}

pthread_mutex_t*  GenericGrid::getLock()
{
    return &lock;
}

void GenericGrid::lockGrid()
{
    pthread_mutex_lock(&lock);
}

void GenericGrid::releaseGrid()
{
    pthread_mutex_unlock(&lock);
}



int GenericGrid::getGridHeight()
{
    return gridHeight;
}
int GenericGrid::getGridWidth()
{
    return gridWidth;
}

void GenericGrid::archBallOverTime(double radius,double r_x, double r_y, int start_x, int stop_x, double width, double timeDuration, RGBColor *color)
{

    double radiusSqr = radius*radius;
    double dx = stop_x > start_x? 0.5 : -0.5;
    double x = start_x;

    while (true)
    {
        double y1 = r_y + sqrt(radiusSqr - pow(x-r_x,2));
        double y2 = r_y - sqrt(radiusSqr - pow(x-r_x,2));
        drawCircle(x,(int)y1,width,color);
        drawCircle(x,(int)y2,width,color);
        usleep((timeDuration) * 1000000/2);
        drawCircle(x,(int)y1,width,RGBColor::BLACK);
        drawCircle(x,(int)y2,width,RGBColor::BLACK);
        x += dx;
        if (dx >0 && x>stop_x)
        {
            break;
        }
        if (dx <0 && x < stop_x)
        {
            break;
        }
    }
}

void GenericGrid::setPixelAA(int x, int y, double pct, RGBColor *color)
{
    int r = (color->getRed() * pct) / 100;
    int g = (color->getGreen() * pct) / 100;
    int b = (color->getBlue() * pct) / 100;
    //printf("DEBUG: %d, %d:  %f, green: %d\n", x, y, pct, g);
    if (x >=0 && y>=0 && x < gridWidth && y < gridHeight)
    {
        getPixal(x,y)->set(r,g,b);
    }
}

void GenericGrid::plotQuadBezierSegAA(int x0, int y0, int x1, int y1, int x2, int y2, RGBColor *color)
{
    int sx = x2-x1, sy = y2-y1;
    long xx = x0-x1, yy = y0-y1, xy;         /* relative values for checks */
    double dx, dy, err, ed, cur = xx*sy-yy*sx;                /* curvature */

    assert(xx*sx >= 0 && yy*sy >= 0);  /* sign of gradient must not change */

    if (sx*(long)sx+sy*(long)sy > xx*xx+yy*yy)   /* begin with longer part */
    {
        x2 = x0;
        x0 = sx+x1;
        y2 = y0;
        y0 = sy+y1;
        cur = -cur; /* swap P0 P2 */
    }
    if (cur != 0)
    {
        /* no straight line */
        xx += sx;
        xx *= sx = x0 < x2 ? 1 : -1;          /* x step direction */
        yy += sy;
        yy *= sy = y0 < y2 ? 1 : -1;          /* y step direction */
        xy = 2*xx*yy;
        xx *= xx;
        yy *= yy;         /* differences 2nd degree */
        if (cur*sx*sy < 0)                            /* negated curvature? */
        {
            xx = -xx;
            yy = -yy;
            xy = -xy;
            cur = -cur;
        }
        dx = 4.0*sy*(x1-x0)*cur+xx-xy;            /* differences 1st degree */
        dy = 4.0*sx*(y0-y1)*cur+yy-xy;
        xx += xx;
        yy += yy;
        err = dx+dy+xy;               /* error 1st step */
        do
        {
            cur = fmin(dx+xy,-xy-dy);
            ed = fmax(dx+xy,-xy-dy);           /* approximate error distance */
            ed = 255/(ed+2*ed*cur*cur/(4.*ed*ed+cur*cur));
            setPixelAA(x0,y0, ed*fabs(err-dx-dy-xy), color);          /* plot curve */
            if (x0 == x2 && y0 == y2)
                return;/* last pixel -> curve finished */
            x1 = x0;
            cur = dx-err;
            y1 = 2*err+dy < 0;
            if (2*err+dx > 0)                                      /* x step */
            {
                if (err-dy < ed)
                    setPixelAA(x0,y0+sy, ed*fabs(err-dy), color);
                x0 += sx;
                dx -= xy;
                err += dy += yy;
            }
            if (y1)                                                /* y step */
            {
                if (cur < ed)
                    setPixelAA(x1+sx,y0, ed*fabs(cur), color);
                y0 += sy;
                dy -= xy;
                err += dx += xx;
            }
        }
        while (dy < dx);                /* gradient negates -> close curves */
    }
    plotLineWidth(x0,y0, x2,y2, 1.0, color);              /* plot remaining needle to end */
}


// Source: http://members.chello.at/~easyfilter/bresenham.html
void GenericGrid::plotLineWidth(int x0, int y0, int x1, int y1, float wd, RGBColor *color)
{
    int dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1;
    int err = dx-dy, e2, x2, y2;                          /* error value e_xy */
    float ed = dx+dy == 0 ? 1 : sqrt((float)dx*dx+(float)dy*dy);

    for (wd = (wd+1)/2; ; )                                     /* pixel loop */
    {
        setPixelAA(x0,y0,100-max((float)0,100*(abs(err-dx+dy)/ed-wd+1)),color);
        e2 = err;
        x2 = x0;
        if (2*e2 >= -dx)                                             /* x step */
        {
            for (e2 += dy, y2 = y0; e2 < ed*wd && (y1 != y2 || dx > dy); e2 += dx)
                setPixelAA(x0, y2 += sy, 100-max((float)0,100*(abs(e2)/ed-wd+1)),color);
            if (x0 == x1)
                break;
            e2 = err;
            err -= dy;
            x0 += sx;
        }
        if (2*e2 <= dy)                                              /* y step */
        {
            for (e2 = dx-e2; e2 < ed*wd && (x1 != x2 || dx < dy); e2 += dy)
                setPixelAA(x2 += sx, y0, 100-max((float)0,100*(abs(e2)/ed-wd+1)),color);
            if (y0 == y1)
                break;
            err += dx;
            y0 += sy;
        }
    }
}



void GenericGrid::drawCircle(int centerX, int centerY, double radius, RGBColor *color)
{
    int x, y;
    for (x = centerX - radius; x <= centerX+radius; x++)
    {
        for (y = centerY-radius; y <= centerY+radius; y++)
        {
            double distiance = sqrt(pow(x-centerX, 2) + pow(y-centerY,2));
            if (distiance <= radius)
            {
                if (x >= 0 && x < gridWidth && y >=0 && y < gridHeight)
                {
                    getPixal(x,y)->set(color);
                }
            }
        }
    }
}


void GenericGrid::interruptThread()
{
    if (interruptAble)
        this->interrupt = true;
}

void GenericGrid::setBackground(RGBColor *bgColor,int xStart, int yStart,int xEnd, int yEnd)
{
    for (int x = xStart; x < xEnd; x++)
    {
        for (int y = yStart; y < yEnd; y++)
        {
            this->getPixal(x,y)->set(bgColor);
        }
    }
}

void GenericGrid::setBackground(RGBColor *bgColor)
{
    setBackground(bgColor,0,0,this->gridWidth, this->gridHeight);
}

void GenericGrid::showPictureNow(RGBPicture &pict, int posX, int posY, bool hideBlack)
{
    int picWidth, picHeight;
    pict.getSize(picWidth, picHeight);
    int x, y;

    int stopX = (posX + picWidth) > gridWidth? gridWidth - posX : picWidth;
    int stopY = (posY + picHeight) > gridHeight ? gridHeight -posY : picHeight;

    for (x = 0; x < stopX; x++)
    {
        for (y=0; y < stopY; y++)
        {
            int r,g,b;
            pict.getRGB(x,y,r,g,b);
            if (hideBlack && r==0 && g==0 &&b==0)
            {
                // do nothing
            }
            else
            {
                int full_x = x+posX;
                int full_y = y+posY;
                if (full_x < gridWidth && full_x > -1 && full_y > -1 && full_y < gridHeight)
                {
                    this->getPixal(full_x, full_y)->set(r,g,b);
                }
            }
        }
    }
}

void GenericGrid::showPictureDummy(RGBPicture &pict, int posX, int posY, bool hideBlack)
{
    int picWidth, picHeight;
    pict.getSize(picWidth, picHeight);
    int x, y;

    int stopX = (posX + picWidth) > dummy_width? dummy_width - posX : picWidth;
    int stopY = (posY + picHeight) > dummy_height ? dummy_height -posY : picHeight;

    for (x = 0; x < stopX; x++)
    {
        for (y=0; y < stopY; y++)
        {
            int r,g,b;
            pict.getRGB(x,y,r,g,b);
            if (hideBlack && r==0 && g==0 &&b==0)
            {
                // do nothing
            }
            else
            {
                this->getBoard(x+posX, y+posY)->set(r,g,b);
            }
        }
    }
}


void GenericGrid::redrawDisplay()
{
    this->setDisplayPosition(currentX, currentY);
}

void GenericGrid::setDisplayPosition(int xOffset, int yOffset)
{
    this->currentX = xOffset;
    this->currentY = yOffset;
    lockGrid();
    for (int i =0; i < this->gridWidth; i++)
    {
        for (int j = 0; j < this->gridHeight; j++)
        {
            this->getPixal(i,j)->copyFrom(this->getBoard(i+xOffset,j+yOffset));
        }
    }
    releaseGrid();
    // Debug
    //printf("%d    %d\n", xOffset, yOffset);
    //sleep(1);
}
void GenericGrid::testGridLayout()
{
    for (int y =0; y < this->gridHeight; y++)
    {
        for (int x = 0; x< this->gridWidth; x++)
        {
            getPixal(x,y)->set(RGBColor::RED);
        }
        gridSleep(1);
        for (int x = 0; x< this->gridWidth; x++)
        {
            getPixal(x,y)->set(RGBColor::BLACK);
        }
    }


    for (int x = 0; x< this->gridWidth; x++)
    {
        for (int y =0; y < this->gridHeight; y++)
        {
            getPixal(x,y)->set(RGBColor::RED);
        }
        gridSleep(1);
        for (int y = 0; y< this->gridHeight; y++)
        {
            getPixal(x,y)->set(RGBColor::BLACK);
        }
    }
}

void GenericGrid::setDummyBackground(RGBColor *bgColor)
{
    setDummyBackground(bgColor, 0, 0, this->dummy_width, this->dummy_height);
}


void GenericGrid::setDummyBackground(RGBColor *bgColor, int xStart, int yStart, int xEnd, int yEnd)
{
    for (int i = xStart; i < xEnd; i++)
    {
        for (int j = yStart; j < yEnd; j++)
        {
            getBoard(i,j)->set(bgColor);
        }
    }

}

void GenericGrid::wipeToRight(RGBColor *color, double delay)
{
    for (int x=0; x< gridWidth; x++)
    {
        for (int y=0; y< gridHeight; y++)
        {
            this->getPixal(x,y)->set(color);
        }
        gridSleep(delay);
    }
}

void GenericGrid::wipeDown(RGBColor *color, double delay)
{
    for (int y=0; y<gridHeight; y++)
    {
        for (int x=0; x< gridWidth; x++)
        {
            this->getPixal(x,y)->set(color);
        }
        gridSleep(delay);
    }
}

void GenericGrid::countdown(bool scrollMerry)
{
    sprintf(message, "Countdown");

    RGBColor *d[25];
    d[0]=d[1]=d[2]=d[3]=RGBColor::RED;
    d[4]=d[5]=d[6]=d[7]=RGBColor::GREEN;
    d[8]=d[9]=d[10]=d[11]=RGBColor::BLUE;
    d[12]=d[13]=d[14]=d[15]=RGBColor::PURPLE;
    d[16]=d[17]=d[18]=d[19]=RGBColor::ORANGE;

    bool counting = true;
    int i = 0;
    char seconds_c[14];

    while(counting)
    {
        int numseconds = timeInfo->getSecondsUntil();
        if (++i > 100000)
            i = 0;

        for (int x = 0; x < this->gridWidth; x++)
        {
            for (int y = 0; y < this->gridHeight; y++)
            {
                int distance = gjhDistance(this->gridWidth/2, this->gridHeight/2, x, y);
                RGBColor *color = d[(distance+i)%20];
                getBoard(x,y)->set(color);
            }
        }

        int posy = gridHeight/2-10+1;
        if (numseconds > 15 )
        {
            int pos =(gridWidth - 96)/2;
            pos += (drawLetter('B',RGBColor::BLACK,pos,posy) +2);
            pos += (drawLetter('E',RGBColor::BLACK,pos,posy) +2);
            //pos += (drawLetter(' ',RGBColor::BLACK,pos,0) +2);
            pos +=6;
            pos += (drawLetter('L',RGBColor::BLACK,pos,posy) +2);
            pos += (drawLetter('O',RGBColor::BLACK,pos,posy) +2);
            pos += (drawLetter('U',RGBColor::BLACK,pos,posy) +2);
            pos += (drawLetter('D',RGBColor::BLACK,pos,posy) +2);

        }
        else if (gridHeight == 20 && numseconds > 8)   // Small Grid
        {
            writeText(RGBColor::BLACK,4,posy,"ALMOST");
        }
        else if (gridHeight == 20 && numseconds >= 0)   // Small Grid
        {
            writeText(RGBColor::BLACK,10,posy,"THERE");
        }
        else
        {
            //setDummyBackground(RGBColor::BLACK,width/2-10,3,width/2+10,h-2);
            int pos = this->gridWidth/2 - (numseconds > 9 ? 7 : 13);
            sprintf(seconds_c, "%7d", numseconds);
            setDummyBackground(RGBColor::BLACK,gridWidth/2 - 9,posy < 3? 0: posy-3,gridWidth/2 + 11,posy+20);
            pos += (drawLetter(seconds_c[5],RGBColor::WHITE,pos,posy) + 2);
            drawLetter(seconds_c[6],RGBColor::WHITE,pos,posy);
        }
        setDisplayPosition(0,0);
        gridSleep(0.05);
        if (numseconds <= 0 )
            counting = false;

    }

    if (scrollMerry)
    {
        for (int i = 0; i< 15; i++)
        {
            this->scrollMerry();
        }
    }
}

void GenericGrid::scrollMerry()
{
    char text[80];
    if ( timeInfo->isNewYears())
    {
        sprintf(text, "HAPPY NEW YEAR");
    }
    else
    {
        sprintf(text, "MERRY CHRISTMAS");
    }
    scrollText(RGBColor::getRandom(), RGBColor::BLACK, text, 0.02);
}

int GenericGrid::drawLetter(char letter, RGBColor *color, int startX, int startY)
{
    return this->drawLetter(letter,color,startX,startY,true);
}


int GenericGrid::drawLetter(char letter, RGBColor *color, int startX, int startY, bool onDummy, int weight)
{
    int x=0;
    int y=0;
    int offset = 0;
    char d[20][40];
    for (x = 0; x < 20; x++)
    {
        for (y=0; y<40; y++)
        {
            d[x][y]='0';
        }
    }
    if (letter == ' ')
    {
        offset = 7;
    }
    else if (letter == '!')
    {
        for (y=0; y < 10; y++)
        {
            d[0][y] = '1';
            d[1][y] = '1';
        }
        d[0][12] = d[1][12] = '1';
        d[0][13] = d[1][13] = '1';
        offset=3;

    }
    else if (letter == '.')
    {

        d[0][13]=d[0][12]=d[1][13]=d[1][12]='1';
        offset=3;
    }
    else if (letter == ',')
    {

        d[0][13]=d[0][12]=d[1][12]='1';
        offset=3;
    }
    else if (letter == '-')
    {

        d[0][6]=d[1][6]=d[2][6]='1';
        d[0][7]=d[1][7]=d[2][7]='1';
        offset=3;
    }
    else if (letter == '^')
    {
        drawSpecial(startX,startY, GRID_TREE);
        return 20;
    }
    else if (letter == '$')
    {
        drawSpecial(startX,startY, GRID_SNOWMEN);
        return 14;
    }
    else if (letter == '#')
    {
        drawSpecial(startX,startY, GRID_SNOWMEN_REVERSE);
        return 14;
    }
    else if (letter == '\\')
    {
        drawSpecial(startX,startY, GRID_TREE_2);
        return 14;
    }
    else if (letter == '+')
    {
        drawSpecial(startX,startY+3, GRID_DEER);
        return 15;

    }
    else if (letter == '0')
    {
        for (y=3; y<10; y++)
        {
            d[0][y]=d[1][y]=d[7][y]=d[8][y]='1';
        }
        d[3][0]=d[4][0]=d[5][0]='1';
        d[2][1]=d[6][1]='1';
        d[1][2]=d[2][2]=d[6][2]=d[7][2]='1';
        d[1][3]=d[7][3]='1';
        d[1][10]=d[7][10]='1';
        d[1][11]=d[2][11]=d[6][11]=d[7][11]='1';
        d[2][12]=d[6][12]='1';
        d[3][13]=d[4][13]=d[5][13]='1';

        offset=9;
    }
    else if (letter == '1')
    {
        for (y=0; y<14; y++)
        {
            d[2][y]=d[3][y]='1';
        }
        d[1][0]=d[0][1]=d[1][1]='1';
        for (x=0; x<6; x++)
        {
            d[x][13]='1';
        }

        offset=6;
    }
    else if (letter == '2')
    {
        for(x=0; x<9; x++)
        {
            d[x][12]=d[x][13]='1';
            d[x][0]=d[x][1]='1';
        }
        d[0][0]=d[1][0]=d[0][1]='0';
        d[6][0]=d[7][0]=d[8][0]='0';
        d[7][1]=d[8][1]='0';
        d[0][12]=d[8][13]='0';

        d[0][2]=d[1][2]=d[5][2]=d[6][2]=d[7][2]='1';
        d[0][3]=d[7][3]=d[6][3]='1';
        d[6][4]=d[7][4]=d[6][5]=d[7][5]='1';
        d[6][6]=d[5][7]=d[5][8]=d[4][9]='1';
        d[3][10]=d[2][11]=d[8][11]='1';

        offset=9;
    }
    else if (letter == '3')
    {
        d[2][0]=d[3][0]=d[4][0]=d[5][0]='1';
        d[1][1]=d[4][1]=d[5][1]=d[6][1]='1';
        d[0][2]=d[5][2]=d[6][2]='1';
        d[5][3]=d[6][3]='1';
        d[5][4]='1';
        d[3][5]=d[4][5]='1';
        d[2][6]=d[3][6]=d[4][6]=d[5][6]='1';
        d[4][7]=d[5][7]=d[6][7]='1';
        d[5][8]=d[6][8]=d[7][8]='1';
        d[6][9]=d[7][9]='1';
        d[6][10]=d[7][10]='1';
        d[6][11]='1';
        d[0][12]=d[1][12]=d[5][12]=d[6][12]='1';
        for (x=0; x<5; x++)
        {
            d[x][13]='1';
        }

        offset=8;
    }
    else if (letter == '4')
    {
        for(y=0; y<14; y++)
        {
            d[6][y]=d[7][y]='1';
            if (y<10)
                d[y][9]='1';
        }
        d[5][2]=d[4][3]=d[3][4]=d[3][5]='1';
        d[2][6]=d[1][7]=d[1][8]='1';

        offset=10;
    }
    else if (letter =='5')
    {
        for(x=2; x<8; x++)
        {
            d[x][0]=d[x][1]='1';
        }
        //d[2][0]=d[7][1]='0';
        d[1][0]=d[2][0]='1';
        d[1][1] = '1';
        d[1][2]=d[2][2]='1';
        d[1][3]=d[2][3]='1';
        d[1][4]=d[2][4]=d[3][4]='1';
        d[2][5]=d[3][5]=d[4][5]='1';
        d[4][6]=d[5][6]=d[6][6]='1';
        d[6][7]=d[7][7]=d[6][8]=d[7][8]='1';
        d[7][9]=d[7][10]=d[6][11]=d[7][11]='1';
        d[0][12]=d[1][12]=d[5][12]=d[6][12]='1';
        d[0][13]=d[1][13]=d[2][13]=d[3][13]=d[4][13]='1';

        offset=8;
    }
    else if (letter == '6')
    {
        d[7][0]=d[8][0]=d[9][0]='1';
        d[5][1]=d[6][1]=d[4][2]=d[5][2]='1';
        d[4][2]=d[5][2]=d[3][3]=d[4][3]='1';
        d[2][4]=d[3][4]='1';
        d[2][5]=d[3][5]=d[4][5]=d[5][5]=d[6][5]=d[7][5]='1';
        d[1][6]=d[2][6]=d[3][6]=d[7][6]=d[8][6]='1';
        d[7][7]='1';
        for(y=7; y<11; y++)
        {
            d[1][y]=d[2][y]=d[8][y]=d[9][y]='1';
        }
        d[2][11]=d[3][11]=d[8][11]='1';
        d[2][12]=d[3][12]=d[7][12]=d[8][12]='1';
        d[4][13]=d[5][13]=d[6][13]=d[7][13]='1';

        offset=10;
    }
    else if (letter == '7')
    {
        for(x=0; x<9; x++)
        {
            d[x][0]=d[x][1]='1';
        }
        d[0][0]=d[8][1]='0';
        d[0][2]=d[7][2]=d[7][3]='1';
        d[6][4]=d[6][5]=d[6][6]='1';
        d[5][7]=d[5][8]=d[5][9]='1';
        d[4][10]=d[4][11]=d[4][12]='1';
        d[3][13]='1';

        offset=9;
    }
    else if (letter =='8')
    {
        for (x=2; x<7; x++)
        {
            d[x][0]=d[x][13]='1';
        }
        d[1][1]=d[2][1]=d[6][1]=d[7][1]='1';
        d[1][12]=d[2][12]=d[6][12]=d[7][12]='1';

        d[0][2]=d[1][2]=d[7][2]=d[8][2]='1';
        d[0][3]=d[1][3]=d[7][3]=d[8][3]='1';
        d[0][4]=d[1][4]=d[2][4]=d[7][4]=d[8][4]='1';
        d[1][5]=d[2][5]=d[3][5]=d[6][5]=d[7][5]='1';
        d[2][6]=d[3][6]=d[4][6]=d[5][6]='1';
        d[3][7]=d[4][7]=d[5][7]=d[6][7]='1';
        d[1][8]=d[2][8]=d[5][8]=d[6][8]=d[7][8]='1';
        d[0][9]=d[1][9]=d[6][9]=d[7][9]=d[8][9]='1';
        d[0][10]=d[1][10]=d[7][10]=d[8][10]='1';
        d[0][11]=d[1][11]=d[7][11]=d[8][11]='1';

        offset=9;
    }
    else if (letter == '9')
    {
        d[2][0]=d[3][0]=d[4][0]=d[5][0]='1';
        d[1][1]=d[2][1]=d[6][1]=d[7][1]='1';
        d[1][2]=d[6][2]=d[7][2]='1';

        for (y=3; y<8; y++)
        {
            d[0][y]=d[1][y]=d[7][y]=d[8][y]='1';
        }
        d[0][7]='0';
        d[2][6]=d[2][7]='1';
        for (x=2; x<8; x++)
        {
            d[x][8]='1';
        }
        d[6][9]=d[7][9]=d[6][10]='1';
        d[4][11]=d[5][11]=d[3][12]=d[4][12]='1';
        d[0][13]=d[1][13]=d[2][13]='1';

        offset=9;
    }
    else if (letter == 'A')
    {
        d[7][0]=d[6][1]=d[7][1]=d[6][2]=d[7][2]=d[8][2] = '1'; //0, 1, 2
        d[5][3]=d[7][3]=d[8][3] = '1'; // 3
        d[5][4]=d[9][4]=d[8][4] = '1'; //4
        d[5][5]=d[9][5]=d[8][5] = '1'; //5
        d[4][6]=d[9][6]=d[8][6] = '1'; //6
        d[4][7]=d[9][7]=d[10][7] = '1'; // 7
        for (x=3; x<11; x++)
        {
            d[x][8] = '1';
        }
        d[3][9]=d[9][9]=d[10][9] = '1'; //9
        d[2][10]=d[10][10]=d[11][10] = '1'; //10
        d[2][11]=d[10][11]=d[11][11] = '1'; //11
        d[1][12]=d[2][12]=d[10][12]=d[11][12]=d[12][12]='1';
        d[0][13]=d[1][13]=d[2][13]=d[3][13] = '1';
        for (x=9; x<14; x++)
        {
            d[x][13] = '1';
        }


        offset = 14;
    }
    else if (letter == 'B')
    {
        for (x=0; x<10; x++)
        {
            d[x][0]=d[x][13]='1';
        }
        for (x=2; x<10; x++)
        {
            d[x][6]='1';
        }

        for (y=0; y<13; y++)
        {
            d[2][y]=d[3][y] = '1';
        }

        d[9][1]=d[10][1]='1';
        y=2;
        d[10][y]=d[11][y]='1';
        y=3;
        d[10][y]=d[11][y]='1';
        y=4;
        d[10][y]=d[11][y]='1';
        d[9][5]=d[10][5]='1';
        d[9][7]=d[10][7]=d[11][7]='1';
        d[12][8]=d[10][8]=d[11][8]='1';
        y=9;
        d[11][y]=d[12][y]='1';
        y=10;
        d[11][y]=d[12][y]='1';
        y=11;
        d[11][y]=d[12][y]='1';
        d[10][12]=d[11][12]='1';
        d[10][13]='1';

        offset=13;
    }
    else if (letter == 'C')
    {
        for (x=4; x<9; x++)
        {
            d[x][0] = '1';
        }
        d[11][0] = '1';
        d[3][1]=d[4][1]=d[9][1]=d[10][1]=d[11][1] = '1'; // 1
        d[2][2]=d[10][2]=d[11][2] = '1';                 // 2
        d[1][3]=d[2][3]=d[10][3]=d[11][3]=d[11][4] = '1'; // 3,4
        for (y=4; y<10; y++)
        {
            d[0][y] = d[1][y] = '1';
        }
        y=10;
        d[1][y]=d[2][y]=d[11][y] = '1'; // 10
        y=11;
        d[1][y]=d[2][y]=d[10][y] = '1'; // 10
        y=12;
        d[3][y]=d[4][y]=d[5][y]=d[9][y] = '1'; // 10
        for (x=4; x<9; x++)
        {
            d[x][13] = '1';
        }
        offset = 12;
    }
    else if (letter == 'D')
    {
        for (x=0; x<10; x++)
        {
            d[x][0] = d[x][13] = '1';
        }
        for (y=0; y<14; y++)
        {
            d[2][y] = d[3][y] = '1';
        }

        for (y=4; y<10; y++)
        {
            d[12][y] = d[13][y] = '1';
        }

        d[9][1]=d[10][1]=d[11][1] = '1'; //1
        d[12][2]=d[10][2]=d[11][2] = '1'; //2
        d[11][3]=d[12][3] = '1'; //3
        d[11][10]=d[12][10] = '1'; //10
        d[10][11]=d[11][11] = '1'; //11
        d[9][12]=d[10][12] = '1'; //12
        offset = 14;
    }
    else if (letter == 'E')
    {
        for (x=0; x < 12; x++)
        {
            d[x][0] = d[x][13]='1';
        }
        for (y=0; y<14; y++)
        {
            d[2][y] = d[3][y] = '1';
        }
        d[10][1]=d[11][1]=d[11][2] = '1';
        d[12][10]=d[11][11]=d[11][12]=d[10][12]='1';
        for (x=2; x<10; x++)
            d[x][6] = '1';

        for (y=4; y<9; y++)
            d[9][y] = '1';

        offset = 13;
    }
    else if (letter == 'F')
    {
        for (x=0; x<11; x++)
        {
            d[x][0] = '1';
            if (x < 6)
                d[x][13] = '1';
        }
        d[9][1]=d[10][1]=d[10][2] = '1';
        d[8][6]=d[8][5]=d[8][4]=d[8][7]=d[8][8]='1';

        for (y=0; y<14; y++)
        {
            d[2][y]=d[3][y]='1';
        }

        for (x=2; x<9; x++)
        {
            d[x][6]='1';
        }

        offset = 11;
    }
    else if (letter == 'G')
    {
        for (x=4; x<9; x++)
        {
            d[x][0]=d[x][13]='1';
        }
        d[11][0]=d[3][1]=d[4][1]=d[9][1]='1';
        d[10][1]=d[11][1]=d[2][2]='1';
        d[10][2]=d[11][2]=d[11][3]='1';
        d[1][3]=d[2][3]='1';
        for (y=4; y<10; y++)
        {
            d[0][y]=d[1][y]='1';
        }
        d[0][4]=d[0][10]=0;
        d[1][10]=d[2][10] = '1';

        for (x=8; x<14; x++)
        {
            d[x][7]='1';
        }

        for(y=7; y<13; y++)
        {
            d[10][y]=d[11][y]='1';
        }
        d[9][13]=d[9][12]='1';
        d[2][9]=d[1][10]=d[2][10]='1';
        d[1][11]=d[2][11]=d[3][11]='1';
        d[2][12]=d[3][12]=d[4][12]='1';


        offset = 14;
    }
    else if (letter == 'H')
    {
        for (x=0; x<6; x++)
        {
            d[x][0] = d[x][13] = '1';
        }
        for (x=9; x<15; x++)
        {
            d[x][0] = d[x][13] = '1';
        }

        for (x=2; x< 13; x++)
        {
            d[x][6] = '1';
        }

        for (y=0; y<14; y++)
        {
            d[2][y]=d[3][y]=d[11][y]=d[12][y] = '1';
        }
        offset=15;
    }
    else if (letter == 'I')
    {
        for (x=0; x<6; x++)
        {
            d[x][0]=d[x][13]='1';
        }

        for (y=0; y<14; y++)
        {
            d[2][y]=d[3][y]='1';
        }

        offset = 6;
    }
    else if (letter == 'J')
    {
        for (x=2; x<8; x++)
        {
            d[x][0]='1';
        }

        for (y=0; y<12; y++)
        {
            d[4][y]=d[5][y]='1';
        }
        d[0][11]=d[1][11]='1';
        d[0][12]=d[1][12]=d[2][12]=d[4][12]='1';
        d[1][13]=d[2][13]=d[3][13]='1';
        offset=8;
    }
    else if (letter == 'K')
    {
        for (x=0; x<6; x++)
        {
            d[x][0]='1';
            d[x][13]='1';
        }

        for (x=9; x<14; x++)
        {
            d[x][0]='1';
            d[x][13]='1';
        }
        for (y=0; y<14; y++)
        {
            d[2][y]=d[3][y]='1';
        }
        x=5;
        y=6;
        while (y <13)
        {
            d[x][y]=d[x-1][y]=d[x+1][y]='1';
            ++x;
            ++y;
        }

        y=5;
        x=6;
        while (y>0)
        {
            d[x][y]='1';
            --y;
            ++x;
        }
        d[11][1]='1';

        offset = 14;
    }
    else if (letter == 'L')
    {
        for (x=0; x<6; x++)
        {
            d[x][0] = '1';
        }

        for (y=0; y<14; y++)
        {
            d[2][y] = d[3][y] = '1';
        }

        for (x=0; x < 11; x++)
        {
            d[x][13] = '1';
        }

        d[9][12]=d[10][12]=d[10][11]=d[11][11]=d[11][10] = '1';
        offset = 12;
    }
    else if (letter == 'M')
    {
        y=-1;
        ++y;
        d[0][y] = d[1][y] = d[2][y] = d[3][y]= d[4][y] =d[14][y]=d[15][y]=d[16][y]=d[17][y] = '1';
        ++y;
        d[2][y]=d[3][y]=d[4][y] = d[13][y] = d[14][y] = d[15][y] = '1'; //1
        ++y;
        d[2][y]=d[3][y]=d[4][y] =d[5][y]= d[13][y] = d[14][y] = d[15][y] = '1'; //2
        ++y;
        d[2][y]=d[4][y]=d[5][y]=d[12][y]=d[14][y]=d[15][y]='1'; //3
        ++y;
        d[2][y]=d[4][y]=d[5][y]=d[12][y]=d[14][y]=d[15][y]='1'; //4
        ++y;
        d[2][y]=d[4][y]=d[5][y]=d[6][y]=d[12][y]=d[14][y]=d[15][y]='1'; //5
        ++y;
        d[2][y]=d[5][y]=d[6][y]=d[11][y]=d[14][y]=d[15][y]='1'; //6
        ++y;
        d[2][y]=d[5][y]=d[6][y]=d[7][y]=d[11][y]=d[14][y]=d[15][y]='1'; //7
        ++y;
        d[2][y]=d[6][y]=d[7][y]=d[10][y]=d[14][y]=d[15][y]='1'; //8
        ++y;
        d[2][y]=d[6][y]=d[7][y]=d[10][y]=d[14][y]=d[15][y]='1'; //9
        ++y;
        d[2][y]=d[7][y]=d[8][y]=d[9][y]=d[14][y]=d[15][y]='1'; //10
        ++y;
        d[2][y]=d[7][y]=d[8][y]=d[9][y]=d[14][y]=d[15][y]='1'; //11
        ++y;
        d[2][y]=d[7][y]=d[8][y]=d[14][y]=d[15][y]='1'; //12
        ++y;
        d[0][y] = d[1][y] = d[2][y] = d[3][y]= d[4][y] =d[12][y]=d[13][y]=d[14][y]=d[15][y]=d[16][y]=d[17][y] = '1'; //13

        offset=17;
    }
    else if (letter == 'N')
    {
        for (x=0; x<4; x++)
        {
            d[x][0] = '1';
        }

        for (x=11; x<16; x++)
        {
            d[x][0] = '1';
        }

        for (y=0; y<14; y++)
        {
            d[3][y] = d[13][y] = '1';
        }
        for (x=1; x< 6; x++)
        {
            d[x][13] = '1';
        }

        x=2;
        for (y=1; y<13; y++)
        {
            d[x][y] = d[x+1][y] = '1';
            ++x;
        }
        d[10][10]=d[11][11]=d[12][12] = '1';
        d[14][12]=d[14][13] = '0';
        offset=16;
    }
    else if (letter == 'O')
    {

        for (x=4; x<9; x++)
        {
            d[x][0] = d[x][13] = '1';
        }

        for (y=4; y<10; y++)
        {
            d[0][y]=d[1][y]=d[11][y]=d[12][y] = '1';
        }
        d[2][1]=d[3][1]=d[9][1] = d[10][1] = '1';
        d[2][2] = d[10][2]=d[11][2] = '1';
        d[1][3]=d[2][3]=d[10][3]=d[11][3] = '1';
        d[1][10]=d[2][10]=d[10][10]=d[11][10] = '1';
        d[1][11]=d[2][11]=d[10][11] = '1';
        d[2][12]=d[3][12]=d[10][12]=d[9][12] = '1';

        offset = 13;
    }
    else if (letter == 'P')
    {
        for (x=0; x<9; x++)
        {
            d[x][0]='1';
            if (x<6)
                d[x][13]='1';
        }
        for (y=0; y<14; y++)
        {
            d[2][y]=d[3][y]='1';
        }

        for (x=4; x<9; x++)
        {
            d[x][7]='1';
        }
        d[8][1]=d[9][1]=d[9][2]=d[10][2]='1';
        d[9][3]=d[10][3]=d[9][4]=d[10][4]='1';
        d[9][5]=d[10][5]=d[8][6]=d[9][6]='1';
        offset=11;
    }
    else if (letter == 'R')
    {
        for (x=0; x<9; x++)
        {
            d[x][0] = '1';
        }
        for (y=0; y<14; y++)
        {
            d[2][y] = d[3][y] = '1';
        }
        d[8][1] = d[9][1]=d[9][2]=d[10][2]='1';
        d[9][3]=d[10][3]=d[9][4]=d[10][4] = '1';
        d[8][5]=d[9][5]='1';
        for (x=4; x<9; x++)
        {
            d[x][6]='1';
        }
        d[6][7]=d[7][7]=d[6][8]=d[7][8]=d[8][8]='1';
        d[7][9]=d[8][9]=d[8][10]=d[9][10]='1';
        d[8][11]=d[9][11]=d[10][11] = '1';
        d[9][12]=d[10][12]=d[11][12] = '1';
        for (x=0; x<6; x++)
        {
            d[x][13] = '1';
        }
        d[12][13]=d[10][13]=d[11][13] =d[13][13]= '1';

        offset = 14;
    }
    else if (letter == 'S')
    {
        y=0;
        d[2][y]=d[3][y]=d[4][y]=d[7][y] = '1';
        y=1;
        d[1][y]=d[2][y]=d[5][y]=d[6][y]=d[7][y] = '1';
        y=2;
        d[0][y]=d[1][y]=d[6][y]=d[7][y] = '1';
        y=3;
        d[0][y]=d[1][y]=d[7][y] = '1';
        y=4;
        d[0][y]=d[1][y]=d[2][y]=d[7][y] = '1';
        y=5;
        d[3][y]=d[1][y]=d[2][y] = '1';
        y=6;
        d[2][y]=d[3][y]=d[4][y]=d[5][y] = '1';
        y=7;
        d[4][y]=d[5][y]=d[6][y] = '1';
        y=8;
        d[5][y]=d[6][y]=d[7][y] = '1';
        y=9;
        d[0][y] = d[6][y]=d[7][y]=d[8][y] = '1';
        y=10;
        d[0][y] = d[7][y]=d[8][y] = '1';
        y=11;
        d[0][y] =d[1][y]= d[7][y]=d[8][y] = '1';
        y=12;
        d[0][y] =d[1][y] =d[2][y]= d[6][y]=d[7][y] = '1';
        y=13;
        d[0][y] =d[3][y] =d[4][y]= d[5][y]=d[6][y] = '1';

        offset = 9;
    }
    else if (letter == 'T')
    {

        for (x=0; x<12; x++)
        {
            d[x][0] = '1';
        }

        for(y=0; y<14; y++)
        {
            d[5][y] = d[6][y] = '1';
        }
        d[0][1]=d[1][1]=d[0][2]=d[10][1]=d[11][1]=d[11][2] = '1';

        for (x=3; x<9; x++)
        {
            d[x][13] = '1';
        }

        offset = 12;
    }
    else if (letter == 'U')
    {
        for (x=0; x<6; x++)
        {
            d[x][0] = '1';
        }
        for (x=10; x<15; x++)
        {
            d[x][0] = '1';
        }

        for(y=1; y<11; y++)
        {
            d[2][y]=d[3][y]=d[12][y] = '1';
        }
        d[3][11]=d[4][11]=d[11][11] = '1';
        d[3][12]=d[4][12]=d[5][12]=d[10][12]=d[11][12] = '1';
        for (x = 5; x<10; x++)
        {
            d[x][13] = '1';
        }

        offset=15;
    }
    else if (letter =='V')
    {
        for (x=0; x<15; x++)
        {
            if (x < 5 || x > 10)
                d[x][0] = '1';
        }
        d[2][1]=d[3][1]=d[4][1]=d[13][1]=d[12][1]='1';
        d[2][2]=d[3][2]=d[12][2]='1';
        d[2][3]=d[3][3]=d[12][3]='1';
        d[4][4]=d[11][4]='1';
        d[4][5]=d[5][5]=d[11][5]='1';
        d[4][6]=d[5][6]=d[11][6]='1';
        d[5][7]=d[6][7]=d[10][7]='1';
        d[5][8]=d[6][8]=d[10][8]='1';
        d[6][9]=d[7][9]=d[9][9]='1';
        d[6][10]=d[7][10]=d[9][10]='1';
        d[6][11]=d[7][11]=d[9][11]='1';
        d[7][12]=d[8][12]=d[7][13]=d[8][13]='1';


        offset=15;
    }
    else if (letter == 'W')
    {
        for (x=0; x< 12; x++)
        {
            d[x][0] = '1';
        }
        y=0;
        d[5][y]=d[6][y]='0';
        d[15][y]=d[16][y]=d[17][y]=d[18][y]='1';

        ++y; //1
        d[1][y]=d[2][y]=d[3][y]=d[8][y]=d[9][y]=d[10][y]='1';
        d[16][y]=d[17][y]='1';

        ++y; //2
        d[2][y]=d[3][y]=d[9][y]=d[10][y]=d[16][y]='1';

        ++y; //3
        d[2][y]=d[3][y]=d[9][y]=d[10][y]=d[16][y]='1';

        ++y; //4
        d[2][y]=d[3][y]=d[8][y]=d[10][y]=d[11][y]=d[16][y]='1';

        ++y;//5
        d[3][y]=d[4][y]=d[8][y]=d[10][y]=d[11][y]=d[15][y]='1';

        ++y; //6
        d[3][y]=d[4][y]=d[8][y]=d[10][y]=d[11][y]=d[15][y]='1';

        ++y; //7
        d[3][y]=d[4][y]=d[7][y]=d[11][y]=d[12][y]=d[15][y]='1';

        ++y; //8
        d[4][y]=d[5][y]=d[7][y]=d[11][y]=d[12][y]=d[14][y]=d[15][y]='1';

        ++y; //9
        d[4][y]=d[5][y]=d[7][y]=d[11][y]=d[12][y]=d[14][y]='1';

        ++y; //10
        d[4][y]=d[5][y]=d[6][y]=d[12][y]=d[13][y]=d[14][y]='1';

        ++y; //11
        d[5][y]=d[6][y]=d[12][y]=d[13][y]=d[14][y]='1';

        ++y; //12
        d[5][y]=d[6][y]=d[13][y]='1';

        ++y;
        d[5][y]=d[13][y]='1';

        offset=19;
    }
    else if (letter == 'X')
    {
        for (x=0; x<15; x++)
        {
            d[x][0]=d[x][13]='1';
        }
        d[7][0]=d[8][0]=d[7][13]=d[5][13]=d[6][13]='0';
        d[2][1]=d[3][1]=d[4][1]=d[11][1]=d[12][1]='1';
        d[3][2]=d[4][2]=d[10][2]=d[11][2]='1';
        d[4][3]=d[5][3]=d[10][3]='1';
        d[5][4]=d[6][4]=d[9][4]='1';
        d[5][5]=d[6][5]=d[7][5]=d[8][5]='1';
        d[6][6]=d[7][6]='1';
        d[7][7]=d[8][7]='1';
        d[6][8]=d[7][8]=d[8][8]=d[9][8] = '1';
        d[5][9]=d[8][9]=d[9][9] = '1';
        d[4][10]=d[5][10]=d[9][10]=d[10][10] = '1';
        d[3][11]=d[4][11]=d[10][11]=d[11][11] = '1';
        d[2][12]=d[3][12]=d[10][12]=d[11][12]=d[12][12] = '1';


        offset=15;
    }

    else if (letter == 'Y')
    {

        for (x=0; x<15; x++)
        {
            d[x][0] = '1';
        }
        d[7][0]=d[8][0]=d[9][0] = '0';

        d[2][1]=d[3][1]=d[4][1]=d[12][1]='1';
        d[3][2]=d[4][2]=d[11][2]='1';
        d[4][3]=d[5][3]=d[11][3]='1';
        d[5][4]=d[6][4]=d[10][4]='1';
        d[5][5]=d[6][5]=d[9][5]='1';
        d[6][6]=d[7][6]=d[9][6]='1';
        for (y=7; y<14; y++)
        {
            d[7][y]=d[8][y]='1';
        }
        for(x=5; x<11; x++)
        {
            d[x][13]='1';
        }

        offset=15;
    }
    else if (letter == 'Z')
    {
        for (x=0; x<10; x++)
        {
            d[x][0]=d[x][13]='1';
        }
        d[0][1]=d[1][1]=d[7][1]=d[8][1]='1';
        d[0][2]=d[7][2]=d[8][2]='1';
        d[0][3]=d[6][3]=d[7][3]='1';
        d[5][4]=d[6][4]=d[5][5]=d[6][5]='1';
        d[4][6]=d[5][6]=d[3][7]=d[4][7]='1';
        d[3][8]=d[4][8]=d[2][9]=d[3][9]='1';
        d[1][10]=d[2][10]=d[10][10]='1';
        d[0][11]=d[1][11]=d[10][11]='1';
        d[0][12]=d[1][12]=d[9][12]=d[10][12]='1';

        offset = 11;
    }

    else if (letter == 0x27)  // ' character
    {
        d[1][0]=d[2][0]=d[1][1]=d[2][1] = '1';
        d[0][2]=d[1][2] = '1';
        offset=3;
    }
    else if (letter == ':')
    {
        d[0][3] = d[1][3] = d[0][4] = d[1][4] = '1';
        d[0][8] = d[1][8] = d[0][9] = d[1][9] = '1';
        offset = 3;
    }

    else
    {
        // ?
        d[2][0]=d[3][0]=d[4][0]=d[5][0] = '1';
        d[1][1]=d[2][1]=d[5][1]=d[6][1] = '1';
        for (y = 2; y< 5; y++)
        {
            d[0][y]=d[1][y]=d[6][y]=d[7][y] = '1';
        }
        d[5][5]=d[6][5]=d[5][6]=d[6][6]=d[5][7]='1';
        d[4][8]=d[3][9]=d[3][10] = '1';
        d[3][12]=d[4][12]=d[3][13]=d[4][13]='1';


        offset=8;

    }

    /*
     * OK, time to really apply changes
     */

    int finalX = startX + (offset*weight);
    if (onDummy)
    {
        if (finalX >= dummy_width)
        {
            // DOn't do it
            return 0;
        }
    }
    else if (finalX >= gridWidth)
    {
        // Don't do it
        return 0;
    }

    // Set colors
    for (x = 0; x < 20; x++)
    {
        for (y=0; y<40; y++)
        {
            if (d[x][y] == '1')
            {
                int realx = startX + (x*weight);
                int realy = startY + (y*weight);
                if (onDummy)
                {
                    for (int i = 0; i < weight; i++)
                    {
                        for (int j = 0; j < weight; j++)
                        {
                            this->getBoard(realx+i, realy+j)->set(color);
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < weight; i++)
                    {
                        for (int j = 0; j < weight; j++)
                        {
                            this->getPixal(realx+i, realy+j)->set(color);
                        }
                    }
                }
            }
        }
    }

    return offset * weight;
}

int GenericGrid::drawLetterSmall(char letter, RGBColor* color, int startX, int startY)
{
    int x=0;
    int y=0;
    int offset = 0;
    char d[12][10];
    for (x = 0; x < 12; x++)
    {
        for (y=0; y<10; y++)
        {
            d[x][y]='0';
        }
    }

    if (letter == 'A')
    {
        d[3][0]=d[3][1]=d[2][2]=d[4][2]='1';
        d[2][3]=d[4][3]='1';
        d[1][4]=d[5][4]=d[1][5]=d[5][5] = '1';
        for (x=1; x< 6; x++)
        {
            d[x][6]='1';
        }
        d[0][7]=d[0][8]=d[6][7]=d[6][8]='1';
        offset = 7;
    }
    else if (letter == 'B')
    {
        for (x=0; x<5; x++)
        {
            d[x][0]=d[x][4]=d[x][8]='1';
        }
        for (y=1; y<8; y++)
        {
            d[0][y]=d[4][y]='1';
        }
        d[4][0]=d[4][4]=d[4][8]='0';

        offset=5;
    }
    else if (letter == 'C')
    {
        for (x=1; x< 5; x++)
        {
            d[x][0]=d[x][8]='1';
        }

        d[5][1]=d[5][7]='1';

        for (int y=1; y<8; y++)
        {
            d[0][y]='1';
        }
        offset = 6;
    }
    else if (letter == 'D')
    {
        for (y=0; y<9; y++)
        {
            d[0][y]='1';
        }
        for (x=0; x<4; x++)
        {
            d[x][0]=d[x][8]='1';
        }
        d[4][1]=d[4][7]='1';
        for (y=2; y<7; y++)
        {
            d[5][y]='1';
        }
        offset = 6;
    }
    else if (letter == 'E')
    {
        for (x=0; x < 5; x++)
        {
            d[x][0] = d[x][4]= d[x][8]='1';
        }
        for (y = 0; y < 8; y++)
        {
            d[0][y]='1';
        }
        d[4][4]='0';
        offset = 5;
    }
    else if (letter == 'F')
    {
        for (x=0; x<5; x++)
        {
            d[x][0]=d[x][4]='1';
        }
        for (y=0; y<9; y++)
        {
            d[0][y]='1';
        }
        d[4][4]='0';
        offset = 5;
    }
    else if (letter == 'G')
    {
        d[1][0]=d[2][0]=d[3][0]=d[4][0] = '1';
        d[0][1]=d[0][2]=d[5][1]=d[0][3]=d[0][4] = '1';
        d[5][4]=d[4][4]=d[3][4]='1';
        d[0][5]=d[5][5]=d[0][6]=d[5][6]='1';
        d[0][7]=d[4][7]=d[5][7]=d[5][8]='1';
        d[1][8]=d[2][8]=d[3][8] = '1';

        offset=6;
    }
    else if (letter == 'H')
    {
        for (x=0; x<6; x++)
        {
            d[x][4] = '1';
        }
        for (y=0; y<9; y++)
        {
            d[0][y]=d[5][y] = '1';
        }

        offset=6;
    }
    else if (letter == 'I')
    {

        for (y=0; y<9; y++)
        {
            d[0][y]='1';
        }

        offset = 1;
    }
    else if (letter == 'K')
    {
        x = 4;
        int dir = -1;
        for (y=0; y<9; y++)
        {
            d[0][y]='1';
            d[x][y]='1';
            x+= dir;
            if (x ==0)
            {
                dir = 1;
                x = 1;
            }
        }

        offset = 6;
    }
    else if (letter == 'L')
    {
        for (y=0; y<9; y++)
        {
            d[0][y]='1';
            d[y][8] = '1';
        }
        d[5][8]=d[6][8]=d[7][8]=d[8][8]='0';

        offset = 5;
    }
    else if (letter == 'M')
    {
        for (y=0; y<9; y++)
        {
            d[0][y]='1';
            d[6][y] = '1';
        }
        d[1][2]=d[5][2]='1';
        d[1][3]=d[5][3]='1';
        d[2][4]=d[4][4]='1';
        d[2][5]=d[4][5]='1';
        d[3][6]='1';

        offset=7;
    }
    else if (letter == 'N')
    {
        for (y=0; y<9; y++)
        {
            d[0][y]=d[5][y]='1';
        }
        d[1][1]=d[1][2]=d[2][3]=d[2][4] = '1';
        d[3][5]=d[4][6]=d[4][7]='1';
        offset=6;
    }
    else if (letter == 'O')
    {
        for (y=1; y<8; y++)
        {
            d[0][y]=d[5][y]='1';
        }
        for (x=1; x<5; x++)
        {
            d[x][0]=d[x][8]='1';
        }
        offset = 6;
    }
    else if (letter == 'P')
    {
        for (x=0; x<5; x++)
        {
            d[x][0] = d[x][4] = '1';
        }
        for (y=1; y< 4; y++)
        {
            d[0][y] = d[5][y] = '1';
        }
        for (y=5; y<9; y++)
        {
            d[0][y] = '1';
        }
        offset = 6;
    }
    else if (letter == 'R')
    {
        for (y=0; y<9; y++)
        {
            d[0][y]=d[5][y]='1';
        }
        for (x=1; x<5; x++)
        {
            d[x][0]=d[x][4]='1';
        }

        d[5][0]=d[5][4]='0';
        offset = 6;
    }
    else if (letter == 'S')
    {
        d[1][0] = d[2][0]=d[3][0] = '1';
        d[0][1]=d[4][1] = '1';
        d[0][2]=d[0][3]='1';
        d[1][4] = d[2][4]=d[3][4] = '1';
        d[4][5]=d[4][6]=d[4][7]=d[0][7] = '1';
        d[1][8] = d[2][8]=d[3][8] = '1';

        offset = 5;
    }
    else if (letter == 'T')
    {
        for (x=0; x<5; x++)
        {
            d[x][0] = '1';
        }

        for (y=1; y<9; y++)
        {
            d[2][y]='1';
        }
        offset = 5;
    }
    else if (letter == 'U')
    {
        for (y=0; y<8; y++)
        {
            d[0][y]=d[5][y]='1';
        }
        d[1][8]=d[2][8]=d[3][8]=d[4][8]='1';
        offset=6;
    }
    else if (letter == 'V')
    {
        d[0][0]=d[0][1]=d[6][0]=d[6][1]='1';
        d[1][2]=d[1][3]=d[1][4]=d[5][2]=d[5][3]=d[5][4]='1';
        d[2][5]=d[2][6]=d[4][5]=d[4][6]='1';
        d[3][7]=d[3][8] = '1';
        offset=7;
    }

    else if (letter == 'W')
    {
        d[0][0]=d[10][0]=d[0][1]=d[10][1]='1';
        d[1][2]=d[5][2]=d[9][2]='1';
        d[1][3]=d[5][3]=d[9][3]='1';
        d[1][4]=d[5][4]=d[9][4]='1';
        d[2][5]=d[4][5]=d[6][5]=d[8][5]='1';
        d[2][6]=d[4][6]=d[6][6]=d[8][6]='1';
        d[3][7]=d[7][7]=d[3][8]=d[7][8]='1';
        offset=11;
    }
    else if (letter == 'X')
    {
        for (y=1; y<8; y++)
        {
            d[y-1][y]=d[7-y][y]='1';
        }
        d[0][0]=d[6][0]=d[0][8]=d[6][8]='1';
        offset=7;
    }

    else if (letter == 'Y')
    {

        d[0][0]=d[6][0]=d[0][1]=d[6][1]='1';
        d[1][2]=d[5][2]=d[2][3]=d[4][3]='1';
        d[3][4]=d[3][5]=d[3][6]=d[3][7]=d[3][8]='1';

        offset=7;
    }
    else if (letter == ' ')
    {
        offset=4;
    }
    else if (letter == '-')
    {
        d[0][4]=d[1][4]=d[2][4] = '1';
        offset=3;
    }
    else if (letter == '.')
    {
        d[0][8] = '1';
        offset=2;
    }
    else if (letter == ',')
    {
        d[0][8] = '1';
        d[1][7] = '1';
        offset=2;
    }
    else if (letter == 0x27)
    {
        d[1][0]=d[2][0]=d[1][1] = '1';
        offset=3;
    }
    else
    {
        //?
        d[1][0]=d[2][0]=d[3][0]='1';
        d[0][1]=d[4][1]=d[4][2]=d[4][3]='1';
        d[3][4]=d[2][5]=d[2][6]=d[2][8]='1';

        offset=5;
    }

    for (x = 0; x < 12; x++)
    {
        for (y=0; y<10; y++)
        {
            if (d[x][y] == '1')
            {
                this->getBoard(startX+x, startY+y)->set(color);
            }
        }
    }

    return offset;
}

int GenericGrid::writeTextSmall(RGBColor *fgColor, int x, int y, string str)
{
    for ( std::string::iterator it=str.begin(); it!=str.end(); ++it)
    {
        x += drawLetterSmall(*it,fgColor,x,y) + 1;
    }
    return x;
}

int GenericGrid::writeText(RGBColor *fgColor, int x, int y, string str)
{
    return this->writeText(fgColor,x,y,str, true);
}

void GenericGrid::writeTextNew(RGBColor *fgColor, int x, int y, std::string &text, bool onDummy, int fontSize)
{
    int width = onDummy ? dummy_width : gridWidth;
    int height = onDummy ? dummy_height : gridHeight;

    Magick::Image image(Magick::Geometry(width,height), Magick::Color("black"));
    image.quiet(true);
    image.depth(8);
    //image.font(font);
    image.fontPointsize(fontSize);
    //image.antiAlias(antialias);

    Magick::TypeMetric metrics;
    image.fontTypeMetrics(text, &metrics);
    image.magick("RGB");
    double rr = fgColor->getRed();
    double rg = fgColor->getGreen();
    double rb = fgColor->getBlue();
    rr /= 100.0f;
    rg /= 100.0f;
    rb /= 100.0f;

    // Color to use when drawing letters
    image.fillColor(Magick::Color(Magick::Color::scaleDoubleToQuantum(rr),
                                  Magick::Color::scaleDoubleToQuantum(rg),
                                  Magick::Color::scaleDoubleToQuantum(rb)));


    // Need to fix position
    Magick::Geometry position(0,0, x, y+(fontSize * 0.7));
    //image.annotate(text, Magick::CenterGravity);
    image.annotate(text, position, Magick::NorthWestGravity);
    Magick::Blob blob;
    image.write( &blob );

    uint8_t* imgData = (uint8_t*)blob.data();

    int pos = 0;
    for (y=0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            int r = imgData[pos++];
            int g = imgData[pos++];
            int b = imgData[pos++];

            // Because we don't want to overwrite the background, we don't write if everything is zero
            if (r==0 && g ==0 && b == 0)
            {
                continue;
            }

            RGBLight * pix = onDummy ? this->getBoard(x,y) : this->getPixal(x,y);
            pix->set(r,g,b);
        }
    }
}

int GenericGrid::writeText(RGBColor *fgColor, int x, int y, string str, bool onDummy, int weight)
{
    for ( std::string::iterator it=str.begin(); it!=str.end(); ++it)
    {
        x += drawLetter(*it,fgColor,x,y, onDummy, weight) + (2 * weight);
    }

    return x;
}

void GenericGrid::scrollText(RGBColor *fgColor, RGBColor *bgColor, char * text, double speed, int y)
{
    sprintf(message, "Scroll: %s", text);
    setDummyBackground(bgColor);

    int textLen = strlen(text);
    int pos = gridWidth + 2;
    for (int i = 0; i < textLen; i++)
    {
        pos += drawLetter(text[i],fgColor,pos,y) + 2;
    }

    setDisplayPosition(0,0);

    for (int i = 0; i < pos; i++)
    {
        setDisplayPosition(i,0);
        gridSleep(speed);
    }
}


void GenericGrid::drawSpecial(int startX, int startY, GRID_SPECIAL type)
{

    if (type == GRID_TREE)
    {
        startX+=(gridHeight-3)/2;
        for (int i = 0; i < gridHeight-3; i++)
        {
            int start = startX-i/2;
            int stop = startX+i/2+1;
            while (start < stop)
            {
                this->getBoard(start++, startY+i)->set(RGBColor::GREEN);
            }
        }
        // Add lights
        getBoard(startX, startY+3)->set(RGBColor::RED);
        getBoard(startX, startY+6)->set(RGBColor::PURPLE);
        getBoard(startX-3, startY+7)->set(RGBColor::PURPLE);
        getBoard(startX+2, startY+8)->set(RGBColor::BLUE);
        getBoard(startX+4, startY+10)->set(RGBColor::RED);
        getBoard(startX-4, startY+10)->set(RGBColor::BLUE);
        getBoard(startX-1, startY+11)->set(RGBColor::RED);
        getBoard(startX+2, startY+12)->set(RGBColor::BLUE);
        getBoard(startX-6, startY+13)->set(RGBColor::RED);
        getBoard(startX-4, startY+14)->set(RGBColor::PURPLE);
        getBoard(startX-0, startY+14)->set(RGBColor::PURPLE);
        getBoard(startX+5, startY+15)->set(RGBColor::RED);

        getBoard(startX,startY)->set(RGBColor::YELLOW); // start at top
    }
    else if (type == GRID_SLEIGH)
    {
        int x = 0;
        int y = 0;

        RGBColor * d[15][gridHeight];
        for (int x = 0 ; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                d[x][y]=0;
            }
        }

        d[0][0]=d[1][0]=d[2][0]=d[3][0]=RGBColor::WHITE;
        d[0][1]=d[4][1]=RGBColor::WHITE;
        d[1][1]=d[2][1]=d[3][1]=RGBColor::RED;
//2
        d[0][2]=d[5][2]=RGBColor::WHITE;
        d[11][2]=d[12][2]=RGBColor::WHITE;
        d[1][2]=d[2][2]=d[3][2]=d[4][2]=RGBColor::RED;
//3
        d[0][3]=d[5][3]=RGBColor::WHITE;
        d[10][3]=d[13][3]=RGBColor::WHITE;
        d[1][3]=d[2][3]=d[3][3]=d[4][3]=RGBColor::RED;
        d[11][3]=d[12][3]=RGBColor::RED;
//4
        for (x=1; x<6; x++)
        {
            d[x][4]=RGBColor::RED;
        }
        d[6][4]=d[9][4]=d[12][4]=d[13][4]=RGBColor::WHITE;
        d[10][4]=d[11][4]=RGBColor::RED;
//5 + 6 + 7+8
        for (x=2; x< 12; x++)
        {
            d[x][5]=RGBColor::RED;
            d[x][6]=RGBColor::RED;
            d[x][7]=RGBColor::GREEN;
            d[x][8]=RGBColor::RED;
        }
        d[6][5]=d[7][5]=RGBColor::WHITE;
        d[2][8]=d[11][8]=0; // Blank
        d[12][8]=d[13][8]=RGBColor::BLACK;

//9
        d[4][9]=d[9][9]=d[13][9]=RGBColor::BLACK;
//10
        d[3][10]=d[4][10]=d[9][10]=d[10][10]=d[12][10]=d[13][10]=RGBColor::BLACK;

//11
        for (x=0; x<13; x++)
        {
            d[x][11]=RGBColor::BLACK;
        }

        // Write it out

        for (x =0; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type==GRID_PRESENT_RED || type == GRID_PRESENT_BLUE || type == GRID_PRESENT_GREEN || type == GRID_PRESENT_PURPLE)
    {

        int x;
        int y;
        RGBColor *baseColor = RGBColor::RED;
        RGBColor *bowColor = RGBColor::WHITE;

        if (type == GRID_PRESENT_GREEN)
            baseColor = RGBColor::GREEN;
        else if (type == GRID_PRESENT_BLUE)
            baseColor = RGBColor::BLUE;
        else if (type == GRID_PRESENT_PURPLE)
            baseColor = RGBColor::PURPLE;

        RGBColor * d[15][gridHeight];
        for (int x = 0 ; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                d[x][y]=0;
            }
        }

        // Draw Present

        for (x=1; x<11; x++)
        {
            for (y=4; y<15; y++)
            {
                d[x][y]=baseColor;
            }
        }
        for (x=1; x<11; x++)
        {
            d[x][8]=d[x][9]=bowColor;
        }
        for (y=2; y<15; y++)
        {
            d[5][y]=d[6][y]=bowColor;
        }
        d[3][1]=d[4][1]=d[7][1]=d[8][1]=bowColor;
        d[3][2]=d[8][2]=bowColor;
        d[4][3]=d[7][3]=bowColor;

        // Write it out

        for (x =0; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }


    else if (type==GRID_TREE_2)
    {
        int x;
        int y;
        RGBColor * d[15][gridHeight];
        for (int x = 0 ; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                d[x][y]=0;
            }
        }

        // start tree
        d[6][0]=d[7][0]=d[6][2]=d[7][2]=RGBColor::YELLOW;
        d[5][1]=d[6][1]=d[7][1]=d[8][1]=RGBColor::YELLOW;

        y=3;
        while (y<7)
        {
            for (x=6-y+3; x<y+5; x++)
            {
                d[x][y]=RGBColor::GREEN;
            }
            ++y;
        }

        y=7;
        while (y<11)
        {
            for (x=12-y; x<y+2; x++)
            {
                d[x][y]=RGBColor::GREEN;
            }
            ++y;
        }

        y=11;
        while (y<15)
        {
            for (x=15-y; x<y-1; x++)
            {
                d[x][y]=RGBColor::GREEN;
            }
            ++y;
        }

        for (y =15; y<18; y++)
        {
            d[5][y]=d[6][y]=d[7][y]=d[8][y]=RGBColor::BROWN;
        }
        for (x=3; x<11; x++)
        {
            d[x][18]=RGBColor::RED;
        }
        d[2][7]=d[11][7]=d[1][11]=d[12][11]=RGBColor::YELLOW;
        d[0][15]=d[13][15]=RGBColor::YELLOW;
        d[3][16]=d[10][16]=RGBColor::RED;
        d[3][17]=d[4][17]=d[9][17]=d[10][17]=RGBColor::RED;

        d[7][4]=d[6][5]=d[5][6]=RGBColor::RED;

        x=8;
        y=7;
        while (y < 12)
        {
            d[x][y]=RGBColor::RED;
            --x;
            ++y;
        }

        x=9;
        y=11;
        while (y < 15)
        {
            d[x][y]=RGBColor::RED;
            --x;
            ++y;
        }


        //END TREE


        for (x =0; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type ==GRID_CANDY)
    {
        int x;
        int y;
        RGBColor * d[15][gridHeight];
        for (int x = 0 ; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                d[x][y]=0;
            }
        }
        for (x=4; x<11; x++)
        {

            d[x][0]=RGBColor::GREY;
        }
//1
        d[3][1]=d[4][1]=RGBColor::GREY;
        d[5][1]=d[6][1]=RGBColor::RED;
        d[10][1]=RGBColor::RED;
        d[11][1]=d[12][1]=RGBColor::GREY;
        d[7][1]=d[8][1]=d[9][1]=RGBColor::WHITE;
//2
        d[2][2]=d[13][2]=RGBColor::GREY;
        d[5][2]=d[6][2]=d[7][2]=RGBColor::RED;
        d[11][2]=d[12][2]=RGBColor::RED;
        d[3][2]=d[4][2]=d[8][2]=d[9][2]=d[10][2]=RGBColor::WHITE;
//3
        d[1][3]=d[13][3]=RGBColor::GREY;
        d[2][3]=d[6][3]=d[7][3]=d[8][3]=d[12][3]=RGBColor::RED;
        d[3][3]=d[4][3]=d[5][3]=d[9][3]=d[10][3]=d[11][3]=RGBColor::WHITE;
//4
        d[0][4]=d[6][4]=d[7][4]=d[8][4]=d[9][4]=d[14][4]=RGBColor::GREY;
        d[1][4]=d[2][4]=d[3][4]=d[13][4]=RGBColor::RED;
        d[4][4]=d[5][4]=d[10][4]=d[11][4]=d[12][4]=RGBColor::WHITE;
//5
        d[0][5]=d[4][5]=d[5][5]=d[10][5]=d[14][5]=RGBColor::GREY;
        d[2][5]=d[3][5]=RGBColor::RED;
        d[1][5]=d[11][5]=d[12][5]=d[13][5]=RGBColor::WHITE;
//6
        d[0][6]=d[3][6]=d[10][6]=d[14][6]=RGBColor::GREY;
        d[1][6]=d[2][6]=d[12][6]=d[13][6]=RGBColor::WHITE;
        d[11][6]=RGBColor::RED;
//7
        d[1][7]=d[2][7]=d[10][7]=d[14][7]=RGBColor::GREY;
        d[11][7]=d[12][7]=RGBColor::RED;
        d[13][7]=RGBColor::WHITE;
//8
        d[9][8]=d[14][8]=RGBColor::GREY;
        d[10][8]=RGBColor::WHITE;
        d[11][8]=d[12][8]=d[13][8]=RGBColor::RED;
//9
        d[9][9]=d[13][9]=RGBColor::GREY;
        d[10][9]=d[11][9]=RGBColor::WHITE;
        d[12][9]=RGBColor::RED;
//10
        d[8][10]=d[13][10]=RGBColor::GREY;
        d[9][10]=RGBColor::RED;
        d[10][10]=d[11][10]=d[12][10]=RGBColor::WHITE;
//11
        d[8][11]=d[12][11]=RGBColor::GREY;
        d[9][11]=d[10][11]=RGBColor::RED;
        d[11][11]=RGBColor::WHITE;
//12
        d[7][12]=d[12][12]=RGBColor::GREY;
        d[8][12]=RGBColor::WHITE;
        d[9][12]=d[10][12]=d[11][12]=RGBColor::RED;
//13
        d[6][13]=d[11][13]=RGBColor::GREY;
        d[7][13]=d[8][13]=d[9][13]=RGBColor::WHITE;
        d[10][13]=RGBColor::RED;
//14
        d[5][14]=d[11][14]=RGBColor::GREY;
        d[6][14]=d[7][14]=RGBColor::RED;
        d[8][14]=d[9][14]=d[10][14]=RGBColor::WHITE;
//15
        d[5][15]=d[10][15]=RGBColor::GREY;
        d[6][15]=d[7][15]=d[8][15]=RGBColor::RED;
        d[9][15]=RGBColor::WHITE;
//16
        d[4][16]=d[10][16]=RGBColor::GREY;
        d[5][16]=d[6][16]=RGBColor::WHITE;
        d[7][16]=d[8][16]=d[9][16]=RGBColor::RED;
//17
        d[4][17]=d[9][17]=RGBColor::GREY;
        d[5][17]=d[6][17]=d[7][17]=RGBColor::WHITE;
        d[8][17]=RGBColor::RED;
//18
        d[4][18]=d[8][18]=RGBColor::GREY;
        d[5][18]=RGBColor::RED;
        d[6][18]=d[7][18]=RGBColor::WHITE;
//19
        d[5][19]=d[6][19]=d[7][19]=RGBColor::GREY;

        for (x =0; x<15; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type == GRID_DEER || type == GRID_DEER_2 ||type == GRID_DEER_2_RED)
    {
        int x=0;
        int y=0;
        RGBColor *d[15][14] ;
        RGBColor *noseColor = RGBColor::BROWN;
        if (type == GRID_DEER_2_RED || type == GRID_DEER)
            noseColor = RGBColor::RED;

        for (x=0; x<15; x++)
        {
            for (y=0; y<14; y++)
            {
                d[x][y]=0;
            }
        }

//0
        d[7][0]=d[11][0]=RGBColor::BROWN;
//1
        d[8][1]=d[9][1]=d[12][1]=RGBColor::BROWN;
//2
        d[9][2]=d[10][2]=d[11][2]=d[14][2]=RGBColor::BROWN;
//3
        d[6][3]=RGBColor::LIGHTBROWN;
        d[9][3]=d[13][3]=RGBColor::BROWN;
//4
        d[6][4]=d[7][4]=RGBColor::LIGHTBROWN;
        d[8][4]=d[9][4]=d[11][4]=d[12][4]=d[13][4]=d[14][4]=RGBColor::BROWN;
//5
        d[6][5]=d[8][5]=RGBColor::LIGHTBROWN;
        d[9][5]=d[11][5]=RGBColor::BROWN;
//6+7
        for(x=6; x<12; x++)
        {
            d[x][6]=d[x][7]=RGBColor::LIGHTBROWN;
        }
        d[10][6]=RGBColor::BROWN;
        d[5][7]=d[12][7]=RGBColor::LIGHTBROWN;
//8
        d[4][8]=d[5][8]=d[8][8]=d[9][8]=d[10][8]=d[12][8]=RGBColor::LIGHTBROWN;
        d[6][8]=d[7][8]=d[11][8]=RGBColor::WHITE;
//9
        for(x=3; x<13; x++)
        {
            d[x][9]=RGBColor::LIGHTBROWN;
        }
        d[6][9]=RGBColor::BLACK;
        d[7][9]=d[10][9]=d[11][9]=RGBColor::WHITE;
//10
        d[1][10]=noseColor;
        for(x=2; x<12; x++)
        {
            d[x][10]=RGBColor::LIGHTBROWN;
        }
        d[5][10]=d[6][10]=RGBColor::BLACK;
        d[7][10]=RGBColor::WHITE;
//11
        d[0][11]=d[1][11]=noseColor;
        for(x=4; x<11; x++)
        {
            d[x][11]=RGBColor::LIGHTBROWN;
        }
        d[5][11]=d[6][11]=RGBColor::WHITE;
//12+13
        for(x=4; x<11; x++)
        {
            d[x][12]=d[x][13]=RGBColor::LIGHTBROWN;
        }

        for (x =0; x<15; x++)
        {
            // reverse image if needed
            int realX = ((type == GRID_DEER_2 || type == GRID_DEER_2_RED) ? 15-x-1 : x);

            for (y=0; y<14; y++)
            {
                if (d[x][y] != 0)
                    getBoard(realX+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type==GRID_SNOWMEN || type ==GRID_SNOWMEN_REVERSE)
    {
        int gridWidth = 15;
        int x=0;
        int y=0;

        RGBColor *d[gridWidth][gridHeight];
        for (x=0; x<gridWidth; x++)
        {
            for (y=0; y< gridHeight; y++)
            {
                d[x][y]=0;
            }
        }
        y=0;
        while (y <4)
        {
            for (x=4-y; x<9+y; x++)
            {
                d[x][y]=RGBColor::WHITE;
                d[x][8-y]=RGBColor::WHITE;
            }
            ++y;
        }

        y=9;
        while(y<13)
        {
            for (x=12-y; x<= y; x++)
            {
                d[x][y]=RGBColor::WHITE;
                d[x][27-y]=RGBColor::WHITE;
            }
            ++y;
        }

        for (x=0; x<13; x++)
        {
            if (x >0 && x< 12)
                d[x][4]=RGBColor::WHITE;
            d[x][13]=d[x][14]=d[x][15]=RGBColor::WHITE;
        }

        // eye + Nose
        d[8][3]=RGBColor::BLACK;
        d[13][4]=d[12][4]=RGBColor::ORANGE;

        //Write it out
        for (x =0; x<gridWidth; x++)
        {
            for (y=0; y<gridHeight; y++)
            {
                // Handle reverse
                int realX = (type == GRID_SNOWMEN ? x : gridWidth-x-1);
                if (d[x][y] != 0)
                    getBoard(realX+startX,y+startY)->set(d[x][y]);
            }
        }
    }
}

void GenericGrid::spiral(RGBColor *color)
{
    int N = this->gridHeight * this->gridWidth * 2.5;
    int x = 0;
    int y = 0;
    for(int i = 0; i < N; ++i)
    {
        int posX = gridWidth/2 + x;
        int posY = gridHeight/2 + y;
        if (posX >= 0 && posX < gridWidth && posY >= 0 && posY < gridHeight)
        {
            gridSleep(N > (gridHeight*gridWidth) ? 0.0001 : 0.001);
            getPixal(posX, posY)->fadeTo(color->getRed(), color->getGreen(), color->getBlue(), 2.5);

        }
        if(abs(x) <= abs(y) && (x != y || x >= 0))
            x += ((y >= 0) ? 1 : -1);
        else
            y += ((x >= 0) ? -1 : 1);
    }
    gridSleep(3.0);
}

int GenericGrid::_gridSleep(double d)
{
    if (this->interrupt && interruptAble)
    {
        this->interrupt = false;
        return 1;
    }
    while (d > 0)
    {
        if (d <= 0.5)
        {
            usleep((d) * 1000000);
            d = 0;
        }
        else
        {
            usleep((0.5) * 1000000);
            d -= 0.5;
            if (d < 0)
                d = 0;
        }
        if (this->interrupt)
        {
            this->interrupt = false;
            return 1;
        }
    }


    return 0;
}

void GenericGrid::colorAroundPicture(RGBPicture *pict, int duration)
{
    sprintf(message, "ColorARoundPicture");
    int picW, picH;
    pict->getSize(picW, picH);
    int posW = (gridWidth - picW) / 2;
    int posH = (gridHeight - picH) /2;

    RGBColor *d[25];
    d[0]=d[1]=d[2]=d[3]=RGBColor::RED;
    d[4]=d[5]=d[6]=d[7]=RGBColor::GREEN;
    d[8]=d[9]=d[10]=d[11]=RGBColor::BLUE;
    d[12]=d[13]=d[14]=d[15]=RGBColor::PURPLE;
    d[16]=d[17]=d[18]=d[19]=RGBColor::ORANGE;

    int i = duration;

    while(--i > 0)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            for (int y = 0; y < gridHeight; y++)
            {
                int distance = gjhDistance(gridWidth/2, gridHeight/2, x, y);
                RGBColor *color = d[(distance+i)%20];
                getBoard(x,y)->set(color);
            }
        }
        showPictureDummy(*pict,posW,posH,true);
        setDisplayPosition(0,0);
        gridSleep(0.05);
    }
    for (int x=0; x<gridWidth; x++)
    {
        for (int y=0; y<gridHeight; y++)
        {
            this->getPixal(x,y)->fadeTo(0,0,0,1);
        }
    }
    gridSleep(1);
}


void GenericGrid::candyCane()
{
    sprintf(message, "Psychedelic Candy Cane");

    RGBColor *d[25];
    d[0]=d[1]=d[2]=d[3]=RGBColor::RED;
    d[4]=d[5]=d[6]=d[7]=RGBColor::GREEN;
    d[8]=d[9]=d[10]=d[11]=RGBColor::BLUE;
    d[12]=d[13]=d[14]=d[15]=RGBColor::PURPLE;
    d[16]=d[17]=d[18]=d[19]=RGBColor::ORANGE;

    int i = 150;

    while(--i > 0)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            for (int y = 0; y < gridHeight; y++)
            {
                int distance = gjhDistance(gridWidth/2, gridHeight/2, x, y);
                RGBColor *color = d[(distance+i)%20];
                getBoard(x,y)->set(color);
            }
        }
        drawSpecial(gridWidth/2-7,gridHeight/2-10,GRID_CANDY);
        setDisplayPosition(0,0);
        gridSleep(0.05);
    }
    for (int x=0; x<gridWidth; x++)
    {
        for (int y=0; y<gridHeight; y++)
        {
            this->getPixal(x,y)->fadeTo(0,0,0,1);
        }
    }
    gridSleep(1.5);
}

void GenericGrid::showPictureNowCenter(RGBPicture &pic,bool hideBlack)
{
    int w,h;
    pic.getSize(w,h);
    int y = (h > gridHeight ? 0 : (gridHeight-h)/2);
    int x = (w > gridWidth ? 0: (gridWidth - w)/2);
    showPictureNow(pic,x,y,hideBlack);
}


void GenericGrid::showMovieCenter(std::string &startsWith, int cnt, double duration)
{
    vector<RGBPicture *> pics;
    RGBPicture::findStartsWith(startsWith, pics);
    RGBPicture * p = pics.at(0);
    int w,h;
    p->getSize(w,h);

    int y = (h > gridHeight ? 0 : (gridHeight-h)/2);
    int x = (w > gridWidth ? 0: (gridWidth - w)/2);
    this->showMovie(startsWith,cnt,duration,x,y);

}

void GenericGrid::showMovie(string &startsWith, int cnt, double duration, int x, int y, bool setBlackFirst)
{
    vector<RGBPicture *> pics;
    if (setBlackFirst)
    {
        this->setBackground(RGBColor::BLACK);
    }
    RGBPicture::findStartsWith(startsWith, pics);
    sprintf(message, "Show Movie: %s", startsWith.c_str());

    for (int i =0 ; i < cnt; i++)
    {
        vector<RGBPicture *>::iterator it;

        for(it = pics.begin(); it != pics.end(); it++)
        {
            RGBPicture *p = (*it);
            this->showPictureNow(*p,x,y,false);
            gridSleep(duration);
        }
    }
}

void GenericGrid::scrollPictureLeft(RGBPicture &pic, double delay, bool hideBlack)
{
    int x, picWidth, picHeight;

    pic.getSize(picWidth,picHeight);
    this->setDummyBackground(RGBColor::BLACK,0,0,picWidth*2 + gridWidth,gridHeight);
    showPictureDummy(pic,gridWidth,0,hideBlack);

    this->setDisplayPosition(0,0);
    for (x = 0; x  < picWidth + this->gridWidth + 1; x++)
    {
        this->setDisplayPosition(x,0);
        gridSleep(delay);
    }
}

void GenericGrid::scrollPictureRight(RGBPicture &pic, double delay, bool hideBlack)
{
    int x, picWidth, picHeight;

    pic.getSize(picWidth,picHeight);
    this->setDummyBackground(RGBColor::BLACK,0,0,picWidth*2 + gridWidth,gridHeight);
    showPictureDummy(pic,gridWidth,0,hideBlack);

    this->setDisplayPosition(0,0);
    for (x = picWidth+gridWidth + 1; x >= 0; --x)
    {
        this->setDisplayPosition(x,0);
        gridSleep(delay);
    }
}

void GenericGrid::scrollPictureUp(RGBPicture &pic, double delay, bool hideBlack)
{
    int y, picWidth, picHeight;
    pic.getSize(picWidth,picHeight);
    int x = (gridWidth - picWidth)/2; // Center pic
    x = x < 0 ? 0 : x;

    this->setDummyBackground(RGBColor::BLACK,0,0,gridWidth,picHeight *2 + gridHeight);
    showPictureDummy(pic,x,gridHeight,hideBlack);

    for (y = 0; y < picHeight+gridHeight+1;  ++y)
    {
        this->setDisplayPosition(0,y);
        gridSleep(delay);
    }
}



char *GenericGrid::getMessage()
{
    return message;
}



GenericGrid::~GenericGrid()
{
    //dtor
}
