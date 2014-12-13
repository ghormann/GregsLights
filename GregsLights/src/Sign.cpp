#include "../include/Sign.h"
#include <unistd.h>
#include "../include/controller/DummyBulb.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


Sign::Sign(bool skipTime, E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4, E131Network *n5, E131Network *n6)
{
    sprintf(message, "Waiting....");
    this->timeInfo = new TimeInfo(skipTime, false);
    currentX = 0;
    currentY = 0;
    int cnt = 0;
    this->clear();
    this->generator = new MessageGenerator();
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n1->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n2->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n3->getRGB(i*3);
    }
    for (int i = 0; i < 140; i++)
    {
        this->pixals[cnt++] = n4->getRGB(i*3);
    }
    for (int i = 0; i < 170; i++)
    {
        this->pixals[cnt++] = n5->getRGB(i*3);
    }
    for (int i = 0; i < 140; i++)
    {
        this->pixals[cnt++] = n6->getRGB(i*3);
    }

    // Setup Dummy Pials
    for (int i = 0 ; i < (SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH); i++)
    {
        this->board[i] = new RGBLight(new DummyBulb(), new DummyBulb(), new DummyBulb());
    }
}

Sign::~Sign()
{
    //dtor
}

RGBLight * Sign::getPixal(int i)
{
    if (i < 0 || i >= TOTAL_SIGN_PIXALS)
    {
        throw "Sign::getPixal must be between 0 and TOTAL_SIGN_PIXALS";
    }
    return this->pixals[i];
}

RGBLight *Sign::getPixal(int x, int y)
{
    int xmod = x%2;
    int pos = 0;
    if (xmod == 0)
    {
        pos = x * SIGN_HEIGHT + y;
    }
    else
    {
        pos = x*SIGN_HEIGHT + (SIGN_HEIGHT-1) - y;
    }

    /*
    || FIx the mistakes that occured during wiring
    */
    if (x == 2 && y > 13)
    {
        pos +=1;
    }

    else if (x == 3)
    {
        if (y == 13)
        {
            pos -= 12;
        }
        else if (y > 13)
        {
            pos += 1;
        }

    }
    return getPixal(pos);
}

RGBLight *Sign::getBoard(int x, int y)
{
    int pos = y*SIGN_DUMMY_WIDTH + x;
    if (pos > (SIGN_DUMMY_HEIGHT * SIGN_DUMMY_WIDTH))
    {
        throw "Sign:getBoard: Invalid x and y positions";
    }

    return board[pos];
}

void Sign::staticSecondsToGo(RGBColor *fgColor, RGBColor *bgColor)
{
    for (int i = 0; i < SIGN_WIDTH; i++)

    {
        for (int j = 0; j < SIGN_HEIGHT; j++)
        {
            getBoard(i,j)->set(bgColor);
        }
    }
    int x=1;
    int y=0;
    x +=drawLetterSmall('S', fgColor, x,y) +1;
    x +=drawLetterSmall('E', fgColor, x,y) +1;
    x +=drawLetterSmall('C', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;
    x +=drawLetterSmall('N', fgColor, x,y) +1;
    x +=drawLetterSmall('D', fgColor, x,y) +1;
    x +=drawLetterSmall('S', fgColor, x,y) +1;

    x=8;
    y=11;
    x +=drawLetterSmall('T', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;
    x+=4;
    x +=drawLetterSmall('G', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;

    setDisplayPosition(0,0);

}

void Sign::scrollText(RGBColor *fgColor, RGBColor *bgColor, char * text, double speed)
{
    sprintf(message, "Scroll: %s", text);
    setDummyBackground(bgColor);

    int textLen = strlen(text);
    int pos = 50;
    for (int i = 0; i < textLen; i++)
    {
        pos += drawLetter(text[i],fgColor,pos,0) + 2;
    }

    setDisplayPosition(0,0);

    for (int i = 0; i < pos; i++)
    {
        setDisplayPosition(i,0);
        gjhSleep(speed);
    }
}

int Sign::drawLetter(char letter, RGBColor *color, int startX, int startY)
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
    else if (letter == '.')
    {

        d[0][13]=d[0][12]=d[1][13]=d[1][12]='1';
        offset=3;
    }
    else if (letter == '^')
    {
        drawSpecial(startX,startY, SIGN_TREE);
        return 20;
    }
    else if (letter == '$')
    {
        drawSpecial(startX,startY, SIGN_SNOWMEN);
        return 14;
    }
    else if (letter == '#')
    {
        drawSpecial(startX,startY, SIGN_SNOWMEN_REVERSE);
        return 14;
    }
    else if (letter == '\\')
    {
        drawSpecial(startX,startY, SIGN_TREE_2);
        return 14;
    }
    else if (letter == '+')
    {
        drawSpecial(startX,startY+3, SIGN_DEER);
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
        d[0][1]=d[1][1]='1';
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
        for(y=0; y<13; y++)
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
        d[2][0]=d[7][1]='0';
        d[2][2]='1';
        d[1][3]=d[2][3]=d[3][3]='1';
        d[1][4]=d[2][4]=d[3][4]=d[4][4]=d[5][4]='1';
        d[3][5]=d[4][5]=d[5][5]=d[6][5]='1';
        d[5][6]=d[6][6]=d[7][6]='1';
        d[6][7]=d[7][7]=d[6][8]=d[7][8]='1';
        d[7][9]=d[7][10]=d[6][11]='1';
        d[0][12]=d[1][12]=d[5][12]='1';
        d[0][13]=d[1][13]=d[2][13]=d[3][13]=d[4][13]='1';

        offset=8;
    }
    else if (letter == '6')
    {
        d[7][0]=d[8][0]=d[9][0]='1';
        d[5][1]=d[6][1]=d[4][2]=d[5][2]='1';
        d[4][2]=d[5][2]=d[3][3]=d[4][3]='1';
        d[2][4]=d[3][4]='1';
        d[2][5]=d[4][5]=d[5][5]=d[6][5]=d[7][5]='1';
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

    for (x = 0; x < 20; x++)
    {
        for (y=0; y<40; y++)
        {
            if (d[x][y] == '1')
                this->getBoard(startX+x, startY+y+3)->set(color);
        }
    }

    return offset;

}

int Sign::drawLetterSmall(char letter, RGBColor* color, int startX, int startY)
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
                this->getBoard(startX+x, startY+y)->set(color);
        }
    }

    return offset;
}

void Sign::redrawDisplay()
{
    this->setDisplayPosition(currentX, currentY);
}

void Sign::setDisplayPosition(int xOffset, int yOffset)
{
    this->currentX = xOffset;
    this->currentY = yOffset;
    for (int i =0; i < SIGN_WIDTH; i++)
    {
        for (int j = 0; j < SIGN_HEIGHT; j++)
        {
            this->getPixal(i,j)->copyFrom(this->getBoard(i+xOffset,j+yOffset));
        }
    }
}

void Sign::testGridLayout()
{
    for (int y =0; y < SIGN_HEIGHT; y++)
    {
        for (int x = 0; x< SIGN_WIDTH; x++)
        {
            getPixal(x,y)->set(RGBColor::RED);
        }
        sleep(1);
        for (int x = 0; x< SIGN_WIDTH; x++)
        {
            getPixal(x,y)->set(RGBColor::BLACK);
        }
    }


    for (int x = 0; x< SIGN_WIDTH; x++)
    {
        for (int y =0; y < SIGN_HEIGHT; y++)
        {
            getPixal(x,y)->set(RGBColor::RED);
        }
        sleep(1);
        for (int y = 0; y< SIGN_HEIGHT; y++)
        {
            getPixal(x,y)->set(RGBColor::BLACK);
        }
    }


}

void Sign::flashSecondsToGo(int times, double delay)
{
    RGBColor *bgColor = new RGBColor(0,0,5);
    for (int i =0; i < times; i++)
    {
        staticSecondsToGo(RGBColor::GREEN, bgColor);
        gjhSleep(delay);
        staticSecondsToGo(RGBColor::RED, bgColor);
        gjhSleep(delay);
        staticSecondsToGo(RGBColor::PURPLE, bgColor);
        gjhSleep(delay);
    }
    delete bgColor;

}

void Sign::setDummyBackground(RGBColor *bgColor)
{
    for (int i = 0; i < SIGN_DUMMY_WIDTH; i++)
    {
        for (int j = 0; j < SIGN_DUMMY_HEIGHT; j++)
        {
            getBoard(i,j)->set(bgColor);
        }
    }
}

void Sign::drawSpecial(int startX, int startY, SIGN_SPECIAL type)
{

    if (type == SIGN_TREE)
    {
        startX+=(SIGN_HEIGHT-3)/2;
        for (int i = 0; i < SIGN_HEIGHT-3; i++)
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
    else if (type==SIGN_TREE_2)
    {
        int x;
        int y;
        RGBColor * d[15][SIGN_HEIGHT];
        for (int x = 0 ; x<15; x++)
        {
            for (y=0; y<SIGN_HEIGHT; y++)
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
            for (y=0; y<SIGN_HEIGHT; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type == SIGN_CANDY)
    {
        int x;
        int y;
        RGBColor * d[15][SIGN_HEIGHT];
        for (int x = 0 ; x<15; x++)
        {
            for (y=0; y<SIGN_HEIGHT; y++)
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
            for (y=0; y<SIGN_HEIGHT; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type == SIGN_DEER)
    {
        int x=0;
        int y=0;
        RGBColor *d[15][14] ;
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
        d[1][10]=RGBColor::RED;
        for(x=2; x<12; x++)
        {
            d[x][10]=RGBColor::LIGHTBROWN;
        }
        d[5][10]=d[6][10]=RGBColor::BLACK;
        d[7][10]=RGBColor::WHITE;
//11
        d[0][11]=d[1][11]=RGBColor::RED;
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
            for (y=0; y<14; y++)
            {
                if (d[x][y] != 0)
                    getBoard(x+startX,y+startY)->set(d[x][y]);
            }
        }

    }
    else if (type==SIGN_SNOWMEN || type ==SIGN_SNOWMEN_REVERSE)
    {
        int width = 15;
        int x=0;
        int y=0;

        RGBColor *d[width][SIGN_HEIGHT];
        for (x=0; x<width; x++)
        {
            for (y=0; y< SIGN_HEIGHT; y++)
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

        // eye
        d[8][3]=0;
        d[13][4]=d[12][4]=RGBColor::ORANGE;

        //Write it out
        for (x =0; x<width; x++)
        {
            for (y=0; y<SIGN_HEIGHT; y++)
            {
                // Handle reverse
                int realX = (type == SIGN_SNOWMEN ? x : width-x-1);
                if (d[x][y] != 0)
                    getBoard(realX+startX,y+startY)->set(d[x][y]);
            }
        }
    }
}

void Sign::wipeToRight(RGBColor *color, double delay)
{
    for (int x=0; x< SIGN_WIDTH; x++)
    {
        for (int y=0; y< SIGN_HEIGHT; y++)
        {
            this->getPixal(x,y)->set(color);
        }
        gjhSleep(delay);
    }
}

void Sign::wipeDown(RGBColor *color, double delay)
{
    for (int y=0; y<SIGN_HEIGHT; y++)
    {
        for (int x=0; x< SIGN_WIDTH; x++)
        {
            this->getPixal(x,y)->set(color);
        }
        gjhSleep(delay);
    }
}


char *Sign::getMessage()
{
    return message;
}

void Sign::rotateSecondsToGo()
{
    sprintf(message, "Flashing Seconds To Go");
    RGBColor *bgColor = new RGBColor(5,0,5);
    staticSecondsToGo(RGBColor::GREEN, bgColor);
    delete bgColor;

    sleep(3);

    bgColor = new RGBColor(0,5,0);
    staticSecondsToGo(RGBColor::RED, bgColor);
    delete bgColor;
    sleep(3);
    flashSecondsToGo(1,1.0);
    flashSecondsToGo(1,0.7);
    flashSecondsToGo(2,0.5);
    flashSecondsToGo(2,0.4);
    flashSecondsToGo(2,0.3);
    flashSecondsToGo(4,0.2);
    flashSecondsToGo(6,0.1);
    sleep(5);

    wipeToRight(RGBColor::BLACK, 0.05);
}

void Sign::run()
{
    double textSpeed = 0.04;


    // Not on douring the day
    if (timeInfo->isDayLight())
    {
        sprintf(message, "Sleeping During the day (%02d)",
                timeInfo->getHourOfDay());
        setDummyBackground(RGBColor::BLACK);
        setDisplayPosition(0,0);
        sleep(60);
        return;
    }

    //
    // Do what a sign should do
    //

    setDummyBackground(RGBColor::BLACK);
    setDisplayPosition(0,0);
    checkClear();

    int i = 6;
    while (useMap[i] == 1)
    {
        i = rand() % SIGN_OPTIONS;  // Don't show same message twice in a row.
    }
    useMap[i] = 1;

    switch(i)
    {
    case 0:
    case 1:
    case 2:
        scrollText(RGBColor::getRandom(), RGBColor::BLACK, generator->getMessage(), textSpeed);
        break;
    case 4:
        rotateSecondsToGo();
        break;
    case 5:
        fewTrees();
        break;
    case 6:
        snowballFight();
        break;
    }
}

void Sign::checkClear()
{
    for (int i = 0; i < SIGN_OPTIONS; i++)
    {
        if (useMap[i] == 0) return;
    }
    // If we got this far, everything is used up.  Clear it.
    this->clear();
}


void Sign::clear()
{
    for (int i = 0; i < SIGN_OPTIONS; i++)
    {
        useMap[i] = 0;
    }

}



void Sign::fewTrees()
{
    sprintf(message, "A Few Trees");

    // Few Christimas Trees
    RGBColor *bgColor = new RGBColor(5,0,5);
    setDummyBackground(bgColor);
    int y=SIGN_HEIGHT+5;
    drawSpecial(0,2,SIGN_TREE);
    drawSpecial(16,0, SIGN_CANDY);
    drawSpecial(31,2,SIGN_TREE);
    setDisplayPosition(0,y);
    while(y>=0)
    {
        setDisplayPosition(0,y--);
        gjhSleep(0.06);
    }
    delete bgColor;
    sleep(6);
    wipeDown(RGBColor::BLACK, 0.06);


}

void Sign::moveBall(int x, RGBColor *bgColor, int startY)
{
    int ballY = startY + (abs(24-x)/2)-1;
    if (ballY < startY)
        ballY = startY-1;

    getBoard(x,ballY)->set(RGBColor::WHITE);
    getBoard(x+1,ballY)->set(RGBColor::WHITE);
    getBoard(x,ballY+1)->set(RGBColor::WHITE);
    getBoard(x+1,ballY+1)->set(RGBColor::WHITE);
    redrawDisplay();
    gjhSleep(0.1);
    getBoard(x,ballY)->set(bgColor);
    getBoard(x,ballY+1)->set(bgColor);
    getBoard(x+1,ballY)->set(bgColor);
    getBoard(x+1,ballY+1)->set(bgColor);

}

void Sign::snowballFight()
{
    sprintf(message, "Snowball Fights");

    RGBColor *bgColor = new RGBColor(0,0,10);
    RGBColor *fgColor = RGBColor::PURPLE;
    setDummyBackground(bgColor);
    int y=0;
    int x=8;

    x +=drawLetterSmall('S', fgColor, x,y) +1;
    x +=drawLetterSmall('N', fgColor, x,y) +1;
    x +=drawLetterSmall('O', fgColor, x,y) +1;
    x +=drawLetterSmall('W', fgColor, x,y) +1;

    x=11;
    y=11;
    x +=drawLetterSmall('B', fgColor, x,y) +1;
    x +=drawLetterSmall('A', fgColor, x,y) +1;
    x +=drawLetterSmall('L', fgColor, x,y) +1;
    x +=drawLetterSmall('L', fgColor, x,y) +1;

    y=SIGN_HEIGHT+2;
    x=10;
    //fight
    x +=drawLetterSmall('F', fgColor, x,y) +1;
    x +=drawLetterSmall('I', fgColor, x,y) +1;
    x +=drawLetterSmall('G', fgColor, x,y) +1;
    x +=drawLetterSmall('H', fgColor, x,y) +1;
    x +=drawLetterSmall('T', fgColor, x,y) +1;
    setDisplayPosition(0,0);
    sleep(2);

    y=0;
    while (y < 13)
    {
        setDisplayPosition(0,y++);
        gjhSleep(0.1);
    }
    /*
     * No sleep. moveBall will sleep
     */
    setDisplayPosition(0,y++);


    /*
     * Now draw snowmen and scroll down!
     */
    int snowmanY = SIGN_HEIGHT+15;
    drawSpecial(0,snowmanY,SIGN_SNOWMEN);
    drawSpecial(33,snowmanY,SIGN_SNOWMEN_REVERSE);

    for (int x=15; x<33; x++)
    {
        moveBall(x, bgColor, snowmanY);
        setDisplayPosition(0,y++);
    }
    while (y <= snowmanY)
    {
        setDisplayPosition(0,y++);
        gjhSleep(0.1);
    }
    redrawDisplay();

    for (int i = 0; i < 2; i++)
    {
        for (int x=15; x<33; x++)
        {
            moveBall(x, bgColor,snowmanY);
        }
        redrawDisplay();
        sleep(1);

        for (int x=32; x>14; x--)
        {
            moveBall(x, bgColor, snowmanY);
        }

        redrawDisplay();
        sleep(1);
    }

    /*
     * Fade to black;
     */
    int duration = 2;
    for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
    {
        getPixal(i)->fadeTo(0,0,0,duration);
    }
    sleep(duration);
    delete bgColor;
}

void Sign::test()
{
    while (1)
    {
        timeInfo->setSkipTimeCheck(true);
        /*
        RGBColor *bgColor = new RGBColor(15,0,15);
        setDummyBackground(bgColor);
        drawSpecial(0,0,SIGN_SNOWMEN);
        setDisplayPosition(0,0);
        sleep(1);
        delete bgColor;
        */

        //scrollText(RGBColor::getRandom(), RGBColor::BLACK, generator->getMessage(), 0.04);
        //scrollText(RGBColor::PURPLE, RGBColor::BLACK, "$ SECONDS LEFT #", 0.04);
        run();
    }

    int betweenPixals = 300;
    int betweenColors = 2;
    this->pixals[0]->set(100,100,100);
    sleep(1);
    this->pixals[0]->set(RGBColor::BLACK);


    while (true)
    {
        setDummyBackground(RGBColor::BLACK);
        setDisplayPosition(0,0);



        for (int x = 0; x < SIGN_WIDTH; x++)
        {
            for (int y = 0; y < SIGN_HEIGHT; y++)
            {
                getPixal(x,y)->set(100,0,0);
                usleep(betweenPixals);
            }
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(100,0,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(0,100,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);
        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(0,0,100);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            this->pixals[i]->set(100,100,0);
            usleep(betweenPixals);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,100,0,100,0,0,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(100,0,0,0,100,0,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(0,100,0,0,0,100,betweenColors);
        }
        sleep(betweenColors);

        for (int i = 0; i < TOTAL_SIGN_PIXALS; i++)
        {
            pixals[i]->fade(0,0,100,0,0,0,betweenColors);
        }
        sleep(betweenColors);
        sleep(2);
    }
}
