#ifndef RGBPICTURE_H
#define RGBPICTURE_H

// See: https://stackoverflow.com/questions/2442335/libpng-boostgil-png-infopp-null-not-found

#define png_infopp_NULL (png_infopp)NULL
#define png_voidp_NULL (png_voidp)NULL
#define int_p_NULL (int*)NULL

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
        std::string &getName();
        bool isMovie();
        static std::vector<RGBPicture> &getAllPictures();
        bool operator< ( const RGBPicture &str) const;
        static RGBPicture * getPicture(const std::string &name);
        static void findStartsWith(const std::string &startsWith, std::vector<RGBPicture> &results);
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
