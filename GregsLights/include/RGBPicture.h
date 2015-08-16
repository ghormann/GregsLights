#ifndef RGBPICTURE_H
#define RGBPICTURE_H

#include <png.h>

class RGBPicture
{
    public:
        RGBPicture(char *fileName);
        virtual ~RGBPicture();
        void getRGB(int x, int y, int &r, int &g, int &b);
        void getSize(int &width, int &height);
    protected:
    private:
        int check_if_png(char *file_name );
        int read_png(char *file_name);
        int height;
        int width;
        png_byte* allData;
};

#endif // RGBPICTURE_H
