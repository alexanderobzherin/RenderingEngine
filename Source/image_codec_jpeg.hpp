#pragma once

#include <iostream>
#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include "image_data.hpp"

extern "C"
{
void SaveTextureFileJpeg(rendering_engine::ImageData const& imageData, char const* filename);
}



