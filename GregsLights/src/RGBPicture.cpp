
#include <stdlib.h>

#ifndef ERROR
#define ERROR 1
#endif // ERROR

#define PNG_BYTES_TO_CHECK 4

// Tool for Converting Images
//convert test128.png -colorspace RGB -depth 8 -resize "64x64" test64.txt

#include "../include/RGBPicture.h"

RGBPicture::RGBPicture(char *file_name)
{
    allData = NULL;
    height = 0;
    width = 0;

    if (check_if_png(file_name))
    {
        read_png(file_name);
    }
    else
    {
        printf("Invalid image %s\n", file_name);
        throw "Invalid file";
    }
}

/*
 * Returns Width and height
 */
void RGBPicture::getSize(int &w, int &h) {
    w = this->width;
    h = this->height;
}

RGBPicture::~RGBPicture()
{
    if (allData != NULL) {
        free(allData);
    }
}


void RGBPicture::getRGB(int x, int y, int &r, int &g, int &b) {
    if (x >= width) {
        throw "X is too big";
    }

    if (y >= height) {
        throw "Y is too big";
    }

    int pos = 3 * (x*height + y);
    r = this->allData[pos];
    g = this->allData[pos+1];
    b = this->allData[pos+2];
}


int RGBPicture::check_if_png(char *file_name )
{
    FILE *fp;
    char buf[PNG_BYTES_TO_CHECK];

    /* Open the prospective PNG file. */
    if ((fp = fopen(file_name, "rb")) == NULL)
        return 0;

    /* Read in some of the signature bytes */
    if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK)
        return 0;

    fclose(fp);

    /* Compare the first PNG_BYTES_TO_CHECK bytes of the signature.
       Return nonzero (true) if they match */
    return(!png_sig_cmp((png_bytep) buf, (png_size_t)0, PNG_BYTES_TO_CHECK));
}

int RGBPicture::read_png(char *file_name)  /* We need to open the file */
{
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    png_bytep * row_pointers;
    int hasAlpha = -1;
    int alpha;
    int color_type;
    FILE *fp;

    if ((fp = fopen(file_name, "rb")) == NULL)
        return (ERROR);

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL,NULL,NULL);

    if (png_ptr == NULL)
    {
        fclose(fp);
        return (ERROR);
    }

    /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        return (ERROR);
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        /* If we get here, we had a problem reading the file */
        return (ERROR);
    }

    /* Set up the input control if you are using standard C streams */
    png_init_io(png_ptr, fp);

    /* If we have already read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

    /* Read whole thing into Memory */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, png_voidp_NULL);

    /* Image is ready for use */
    this->width = png_get_image_width(png_ptr, info_ptr);
    this->height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    //bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        alpha = 255;
        hasAlpha = 1;
    }
    else if (color_type == PNG_COLOR_TYPE_RGB)
    {
        hasAlpha = 0;
    }
    else
    {
        printf("ERROR: Unsported color type\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        /* If we get here, we had a problem reading the file */
        return (ERROR);

    }

    //printf("Size: %i x %i, Has Alpha: %d\n", width,height, hasAlpha );

    /*
     * FInally, time to read
     */
    int x, y;
    png_set_sig_bytes(png_ptr, 8);

    /* read file */
    row_pointers = png_get_rows(png_ptr, info_ptr);

    this->allData = (png_byte*) malloc(width * height * 3);
    if (allData == NULL)
    {
        printf("EMalloc failed\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        /* If we get here, we had a problem reading the file */
        return (ERROR);
    }

    for (y=0; y<height; y++)
    {
        png_byte* row = row_pointers[y];
        for (x=0; x<width; x++)
        {
            png_byte* ptr = &(row[x* (hasAlpha ? 4 : 3)]);
            alpha = hasAlpha ? ptr[3] : 255;
            int r = ptr[0] * alpha / 255;
            int g = ptr[1] * alpha / 255;
            int b = ptr[2] * alpha / 255;

            int pos = (x*height+y)*3;
            allData[pos] =r;
            allData[pos+1] = g;
            allData[pos+2] = b;

            /*
                        printf("DEBUG: Pixel at position [ %d - %d ] has RGB values: %d - %d - %d\n",
                               x, y, r, g, b);
            */
        }
    }


    /* Clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    /* Close the file */
    fclose(fp);

    /* That's it */
    return (0);
}
