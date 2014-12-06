#include "../include/Sign.h"
#include <unistd.h>
#include "../include/controller/DummyBulb.h"
#include <stdio.h>
#include <string.h>


Sign::Sign(E131Network *n1, E131Network *n2, E131Network *n3, E131Network *n4, E131Network *n5, E131Network *n6)
{
    currentX = 0;
    currentY = 0;
    int cnt = 0;
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

void Sign::scrollText(RGBColor *fgColor, RGBColor *bgColor, char * text)
{
    for (int i = 0; i < SIGN_DUMMY_WIDTH; i++)
    {
        for (int j = 0; j < SIGN_DUMMY_HEIGHT; j++)
        {
            getBoard(i,j)->set(bgColor);
        }
    }

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
        usleep(50000);
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
    char d[10][10];
    for (x = 0; x < 10; x++)
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

    for (x = 0; x < 10; x++)
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

void Sign::test()
{
    int betweenPixals = 300;
    int betweenColors = 2;
    this->pixals[0]->set(100,100,100);
    sleep(1);
    this->pixals[0]->set(RGBColor::BLACK);

    while (true)
    {
        //flashSecondsToGo(4, 0.4);
        //scrollText(RGBColor::RED, RGBColor::BLACK, "SECONDS UNTIL CHRISTMAS");
        //scrollText(RGBColor::WHITE, RGBColor::BLACK, "MERRY CHRISTMAS FROM THE HORMANN'S");
        //scrollText(RGBColor::GREEN, RGBColor::BLACK, "ARE YOU READY FOR CHRISTMAS?");
        scrollText(RGBColor::GREEN, RGBColor::BLACK, "HAVE YOU BEEN NAUGHTY OR NICE?");


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
