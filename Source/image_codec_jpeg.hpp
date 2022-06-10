#pragma once

#include <iostream>
#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include "image_data.hpp"

extern "C" 
{
void SaveTextureFileJpeg(rendering_engine::ImageData const& imageData, char const* filename)
{
	JSAMPLE* imageBuffer;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE* outfile;                /* target file */
	JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
	int row_stride;               /* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	std::cout << "Hellp from SaveTextureFileJpeg" << std::endl;
	}
}


