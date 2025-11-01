// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include <cstdint>
#include <iostream>
#include <map>
#include <algorithm>
#include "image_data.hpp"
#include "boost/filesystem.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TYPES_H
#include FT_OUTLINE_H
#include FT_RENDER_H

namespace rendering_engine
{

class TextRenderer
{
public:
    TextRenderer( std::string const pathToFont, unsigned int const size );
    ~TextRenderer();

    ImageData CreateGlyphBitmap( char const character );
    //Not effective method. It is not using texAtlas.
    ImageData CreateStringBitmap( std::string text );

    static ImageData CreateGlyphBitmap( std::string const pathToFont, char const character );

private:
    TextRenderer(TextRenderer const&);
    TextRenderer operator=(TextRenderer const &);

protected:
    FT_Error   mErrorResult = FT_Err_Ok;
    FT_Library mLibrary = 0;
    FT_Face    mFace = 0;

    std::map<char, ImageData> mGlyphBitmaps;
    ImageData mGlyphTextureAtlas;

    //TO DO evaluate to store all needed data for each character
    std::map<char, std::pair<unsigned int, unsigned int>> mTexAtlasData;
};

} //namespace rendering_engine