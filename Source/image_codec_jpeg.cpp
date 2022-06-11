#include "image_codec_jpeg.hpp"

extern "C"
{
void SaveTextureFileJpeg(rendering_engine::ImageData const& imageData, char const* filename)
{
	auto const imageDataVector = imageData.GetImageDataRGB();
	JSAMPLE* imageBuffer = new JSAMPLE[imageDataVector.size()];
	std::copy(imageDataVector.begin(), imageDataVector.end(), imageBuffer);
	int quality = 24;

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
}