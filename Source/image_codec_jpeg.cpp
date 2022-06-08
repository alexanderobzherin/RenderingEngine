#include "image_codec_jpeg.hpp"

extern "C"
{
void SaveTextureFileJpeg()
{
	JSAMPLE *imageBuffer;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *outfile;                /* target file */
	JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
	int row_stride;               /* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
}

}