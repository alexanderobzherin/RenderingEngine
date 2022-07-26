#pragma once

#include <cstring>
#include <vector>
#include <stdlib.h>
#include <png.h>

static void SaveTextureFilePng(rendering_engine::ImageData const& imageData, char const* filename)
{
    FILE* fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_colorp palette;
    
    /* Open the file */
    fp = fopen(filename, "wb");
    if( fp == NULL )
    {
        fprintf(stderr, "can't open %s\n", filename);
        exit(1);
    }

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        NULL, NULL, NULL);

    if( png_ptr == NULL )
    {
        fclose(fp);
        return;
    }

    /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if( info_ptr == NULL )
    {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        return;
    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
     * error handling functions in the png_create_write_struct() call.
     */
    if( setjmp(png_jmpbuf(png_ptr)) )
    {
        /* If we get here, we had a problem writing the file */
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    /* One of the following I/O initialization functions is REQUIRED */

    png_init_io(png_ptr, fp);
    auto const png_transforms = PNG_TRANSFORM_IDENTITY;

   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
    int const bit_depth = 8;
    png_set_IHDR(png_ptr, info_ptr, imageData.GetWidth(), imageData.GetHeight(), bit_depth, PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    /* Set the palette if there is one.  REQUIRED for indexed-color images */
    palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
        * sizeof(png_color));
    /* ... Set palette colors ... */
    png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);
    /* You must not free palette here, because png_set_PLTE only makes a link to
     * the palette that you malloced.  Wait until you are about to destroy
     * the png structure.
     */

     /* Optional significant bit (sBIT) chunk */
    png_color_8 sig_bit;

    /* If we are dealing with a grayscale image then */
    //sig_bit.gray = true_bit_depth;

    /* Otherwise, if we are dealing with a color image then */
    sig_bit.red = 8; //true_red_bit_depth;
    sig_bit.green = 8; //true_green_bit_depth;
    sig_bit.blue = 8; // true_blue_bit_depth;

    /* If the image has an alpha channel then */
    sig_bit.alpha = 8; // true_alpha_bit_depth;

    png_set_sBIT(png_ptr, info_ptr, &sig_bit);

     /* Write the file header information.  REQUIRED */
    png_write_info(png_ptr, info_ptr);

    auto const imageDataVector = imageData.GetImageDataRGBA();

    for( auto rowIterator = 0U; rowIterator < imageData.GetHeight(); ++rowIterator )
    {
        png_bytep row_pointer = new unsigned char[imageData.GetWidth() * 4];
        auto const rowLength = imageData.GetWidth() * 4U;
        auto const beginRow = imageDataVector.begin() + (rowIterator * rowLength);
        auto const endRow = imageDataVector.begin() + (rowIterator * rowLength) + rowLength;
        std::copy(beginRow, endRow, row_pointer);

        png_write_row(png_ptr, row_pointer);

        delete[] row_pointer;
    }

     /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);

    /* If you png_malloced a palette, free it here (don't free info_ptr->palette,
     * as recommended in versions 1.0.5m and earlier of this example; if
     * libpng mallocs info_ptr->palette, libpng will free it).  If you
     * allocated it with malloc() instead of png_malloc(), use free() instead
     * of png_free().
     */
    png_free(png_ptr, palette);
    palette = NULL;
    
    /* Similarly, if you png_malloced any data that you passed in with
     * png_set_something(), such as a hist or trans array, free it here,
     * when you can be sure that libpng is through with it.
     */
    //png_free(png_ptr, png_transforms);
    //trans = NULL;
    /* Whenever you use png_free() it is a good idea to set the pointer to
     * NULL in case your application inadvertently tries to png_free() it
     * again.  When png_free() sees a NULL it returns without action, thus
     * avoiding the double-free security problem.
     */

     /* Clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, &info_ptr);

    /* Close the file */
    fclose(fp);
}

static bool ReadPngFile(char const* filename, unsigned int& width, unsigned int& height, std::vector<unsigned int>& rgbaImageDataVector)
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
            width = image.width;
            height = image.height;
            rgbaImageDataVector.clear();
            for( int i = 0; i < width * height * 4; ++i )
            {
                rgbaImageDataVector.push_back(*(buffer + i));
            }

            delete[] buffer;
        }
        else
        {
            return false;
        }
    }
    return true;
}