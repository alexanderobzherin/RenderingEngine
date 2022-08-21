#pragma once

#include <iostream>
#include <unordered_map>
#include <map>
#include <memory>
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