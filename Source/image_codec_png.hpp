#pragma once

#include <cstring>
#include <vector>
#include <stdlib.h>
#include <png.h>

// #ifndef png_jmpbuf
// #  define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
// #endif

// #define PNG_BYTES_TO_CHECK 4

static bool ReadPngFile(char* filename, unsigned int& width, unsigned int& height, std::vector<unsigned int>& rgbImageDataVector)
{
    png_image image; /* The control structure used by libpng */

    /* Initialize the 'png_image' structure. */
    memset(&image, 0, (sizeof image));
    image.version = PNG_IMAGE_VERSION;

    /* The first argument is the file to read: */
    if(png_image_begin_read_from_file(&image, filename) != 0)
    {
        png_bytep buffer;

        /* Set the format in which to read the PNG file; this code chooses a
        * simple sRGB format with a non-associated alpha channel, adequate to
        * store most images.
        */
        image.format = PNG_FORMAT_RGBA;

        /* Now allocate enough memory to hold the image in this format; the
        * PNG_IMAGE_SIZE macro uses the information about the image (width,
        * height and format) stored in 'image'.
        */
        buffer = new png_byte[(PNG_IMAGE_SIZE(image))];

        /* If enough memory was available, read the image in the desired
        * format, then write the result out to the new file.  'background' is
        * not necessary when reading the image, because the alpha channel is
        * preserved; if it were to be removed, for example if we requested
        * PNG_FORMAT_RGB, then either a solid background color would have to
        * be supplied, or the output buffer would have to be initialized to
        * the actual background of the image.
        *
        * The fourth argument to png_image_finish_read is the 'row_stride' -
        * this is the number of components allocated for the image in each
        * row.  It has to be at least as big as the value returned by
        * PNG_IMAGE_ROW_STRIDE, but if you just allocate space for the
        * default, minimum size, using PNG_IMAGE_SIZE as above, you can pass
        * zero.
        *
        * The final argument is a pointer to a buffer for the colormap;
        * colormaps have exactly the same format as a row of image pixels
        * (so you choose what format to make the colormap by setting
        * image.format).  A colormap is only returned if
        * PNG_FORMAT_FLAG_COLORMAP is also set in image.format, so in this
        * case NULL is passed as the final argument.  If you do want to force
        * all images into an index/color-mapped format, then you can use:
        *
        *    PNG_IMAGE_COLORMAP_SIZE(image)
        *
        * to find the maximum size of the colormap in bytes.
        */

        if (buffer != NULL &&
        png_image_finish_read(&image, NULL/*background*/, buffer,
            0/*row_stride*/, NULL/*colormap*/) != 0)
        {
            /* Now write the image out to the second argument.  In the write
                * call 'convert_to_8bit' allows 16-bit data to be squashed down to
                * 8 bits; this isn't necessary here because the original read was
                * to the 8-bit format.
                */
            if (png_image_write_to_file(&image, "outTextureFile.png", 0/*convert_to_8bit*/,
                buffer, 0/*row_stride*/, NULL/*colormap*/) != 0)
            {
                /* The image has been written successfully. */
                exit(0);
            }

            delete[] buffer;
        }
    }

}