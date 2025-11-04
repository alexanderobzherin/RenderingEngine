// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "image_data.hpp"

namespace rendering_engine
{
class TextureAtlasMaker
{
public:
	TextureAtlasMaker( std::map<char, ImageData> images );
	~TextureAtlasMaker() = default;

	bool CreateTextureAtlas( std::map<char, std::pair<unsigned int, unsigned int>>& texAtlasData, ImageData& texAtlasImage );

protected:
	//Calculate cell dimensions of one cell as max width (x component) and height (y component) in pixels
	void FindCellDimensions(unsigned int& outputWidth, unsigned int& outputHeight);
	//Calculate number of rows and columns, trying to aim a square form
	void CalculateGridDimensions(unsigned int& outputNumberOfColumns, unsigned int& outputNumberOfRows);
	//Log informaion to txt file
	void LogMetaData(const std::string& message)
	{
		mMetaDataOutputStream << message << std::endl;
	}

private:
	TextureAtlasMaker( TextureAtlasMaker const& );
	TextureAtlasMaker operator=(TextureAtlasMaker const&);

protected:
	//container of images
	std::map<char, ImageData> mImageCollection;

	static const char* const mMetaDataFileName;
	std::ofstream mMetaDataOutputStream;

	static const Color sDefaultPaletteBackgroundColor;
};

} //namespace rendering_engine