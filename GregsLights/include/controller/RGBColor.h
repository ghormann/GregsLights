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

    protected:
    private:
        int red, green, blue;
};

#endif // RGBCOLOR_H
