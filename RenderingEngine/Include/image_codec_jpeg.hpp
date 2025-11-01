// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

/**
 * @file image_codec_jpeg.hpp
 * @brief JPEG read/write backend using libjpeg.
 *
 * Provides low-level I/O routines bridging `ImageData` and the libjpeg API.
 * Supports 8-bit RGB images for loading and saving.
 *
 * @details
 * The implementation follows the official libjpeg API workflow:
 * - https://libjpeg.sourceforge.io/
 * - Steps: setup error handler -> create compressor/decompressor -> set parameters -> read/write scanlines -> clean up.
 *
 * Custom error handling is implemented via `codec_error_mgr` to safely recover
 * from libjpeg’s internal `longjmp` behavior.
 *
 * @note This file is an internal backend of the Rendering Engine and is not part of the public API.
 *
 * @see rendering_engine::ImageData
 */
#pragma once

#include <iostream>
#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include "image_data.hpp"

 /**
  * @brief Saves image data to a JPEG file.
  * @param imageData Source image in RGB format.
  * @param filename Path to the output file.
  */
static void SaveTextureFileJpeg(rendering_engine::ImageData const& imageData, char const* filename)
{
	auto const imageDataVector = imageData.GetImageDataRGB();
	JSAMPLE* imageBuffer = new JSAMPLE[imageDataVector.size()];
	std::copy(imageDataVector.begin(), imageDataVector.end(), imageBuffer);
	int quality = 100;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE* outfile;                /* target file */
	JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
	int row_stride;               /* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if( (outfile = fopen(filename, "wb")) == NULL ) {
		fprintf(stderr, "can't open %s\n", filename);
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = imageData.GetWidth();   /* image width and height, in pixels */
	cinfo.image_height = imageData.GetHeight();
	cinfo.input_components = 3;					/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;             /* colorspace of input image */

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);

	row_stride = imageData.GetWidth() * 3; /* JSAMPLEs per row in image_buffer */

	while( cinfo.next_scanline < cinfo.image_height )
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = &imageBuffer[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	/* After finish_compress, we can close the output file. */
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);
	delete[] imageBuffer;
}

struct codec_error_mgr {
	struct jpeg_error_mgr pub;    /* "public" fields */

	jmp_buf setjmp_buffer;        /* for return to caller */
};

typedef struct codec_error_mgr* codec_error_mgr_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
codec_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a codec_error_mgr struct, so coerce pointer */
	codec_error_mgr_ptr codecErr = (codec_error_mgr_ptr)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(codecErr->setjmp_buffer, 1);
}

static bool
DoReadJpegFile(struct jpeg_decompress_struct* cinfo,
	char const* filename, unsigned int& width, unsigned int& height, std::vector<unsigned int>& rgbImageDataVector);

/*
 * Sample routine for JPEG decompression.  We assume that the source file name
 * is passed in.  We want to return 1 on success, 0 on error.
 */

 /**
  * @brief Reads a JPEG file into an RGB image buffer.
  * @param filename Path to input JPEG file.
  * @param width [out] Image width in pixels.
  * @param height [out] Image height in pixels.
  * @param rgbImageDataVector [out] Output vector of RGB pixel data (3 bytes per pixel).
  * @return true on success, false on failure.
  */
static bool ReadJpegFile(char const* filename, unsigned int& width, unsigned int& height, std::vector<unsigned int>& rgbImageDataVector)
{
	/* This struct contains the JPEG decompression parameters and pointers to
	 * working space (which is allocated as needed by the JPEG library).
	 */
	struct jpeg_decompress_struct cinfo;

	return DoReadJpegFile(&cinfo, filename, width, height, rgbImageDataVector);
}

/*
 * We call the libjpeg API from within a separate function, because modifying
 * the local non-volatile jpeg_decompress_struct instance below the setjmp()
 * return point and then accessing the instance after setjmp() returns would
 * result in undefined behavior that may potentially overwrite all or part of
 * the structure.
 */

bool
DoReadJpegFile(struct jpeg_decompress_struct* cinfo, char const* filename, unsigned int& width, unsigned int& height, std::vector<unsigned int>& rgbImageDataVector)
{
	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct codec_error_mgr jerr;

	FILE* infile;                 /* source file */
	JSAMPARRAY buffer;            /* Output row buffer */
	int row_stride;               /* physical row width in output buffer */

	if( (infile = fopen(filename, "rb")) == NULL )
	{
		std::cout << "File can not be open." << std::endl;
		return false;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo->err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = codec_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if( setjmp(jerr.setjmp_buffer) ) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(cinfo);
		fclose(infile);
		return false;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */
	
	(void)jpeg_read_header(cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.txt for more info.
	 */

	 /* Step 4: set parameters for decompression */

	 /* In this example, we don't need to change any of the defaults set by
	  * jpeg_read_header(), so we do nothing here.
	  */

	/* Step 5: Start decompressor and fulfill the output data */
	width = cinfo->image_width;
	height = cinfo->image_height;

	rgbImageDataVector.clear();

	(void)jpeg_start_decompress(cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	 /* We may need to do some setup of our own at this point before reading
	  * the data.  After jpeg_start_decompress() we have the correct scaled
	  * output image dimensions available, as well as the output colormap
	  * if we asked for color quantization.
	  * In this example, we need to make an output work buffer of the right size.
	  */
	  /* JSAMPLEs per row in output buffer */
	row_stride = cinfo->output_width * cinfo->output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo->mem->alloc_sarray)
		((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);

	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo->output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	;
	while( cinfo->output_scanline < cinfo->output_height ) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void)jpeg_read_scanlines(cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
		for( int i = 0; i < row_stride; ++i )
		{
			rgbImageDataVector.push_back(*(buffer[0] + i));
		}
	}

	/* Step 7: Finish decompression */

	(void)jpeg_finish_decompress(cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	 /* Step 8: Release JPEG decompression object */

	 /* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
	fclose(infile);

	/* At this point you may want to check to see whether any corrupt-data
	 * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	 */

	 /* And we're done! */
	return true;
}


