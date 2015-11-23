#ifndef RGBCOLOR_H
#define RGBCOLOR_H


class RGBColor
{
    public:
        RGBColor(int r, int g, int b);
        virtual ~RGBColor();
        int getRed();
        int getGreen();
        int getBlue();
        static RGBColor *RED;
        static RGBColor *BLUE;
        static RGBColor *GREEN;
        static RGBColor *PURPLE;
        static RGBColor *BLACK;
        static RGBColor *YELLOW;
        static RGBColor *WHITE;
        static RGBColor *ORANGE;
        static RGBColor *GREY;
        static RGBColor *BROWN;
        static RGBColor *UNKNOWN;
        static RGBColor *LIGHTBROWN;
        static RGBColor *DARKGREEN;
        static RGBColor * getRandom();

    protected:
    private:
        int red, green, blue;
};

#endif // RGBCOLOR_H
