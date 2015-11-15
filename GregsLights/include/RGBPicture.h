#ifndef RGBPICTURE_H
#define RGBPICTURE_H

#include <sys/types.h>
#include <png.h>
#include <string>
#include <vector>

class RGBPicture
{
    public:
        RGBPicture(char *fileName);
        virtual ~RGBPicture();
        void getRGB(int x, int y, int &r, int &g, int &b);
        void getSize(int &width, int &height);
        std::string getName();
        static std::vector<RGBPicture> getAllPictures();
        bool operator< ( const RGBPicture &str) const;
        static RGBPicture * getPicture(const std::string &name);
    protected:
    private:
        int check_if_png(char *file_name );
        int read_png(char *file_name);
        int height;
        int width;
        std::string shortName;
        png_byte* allData;
        static std::vector<RGBPicture> allPictures;
};

#endif // RGBPICTURE_H
