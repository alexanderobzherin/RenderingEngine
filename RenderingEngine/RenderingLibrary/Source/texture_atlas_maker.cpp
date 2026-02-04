#include "texture_atlas_maker.hpp"
#include <cmath>
#include "font_resources.hpp"

namespace rendering_engine
{

const Color TextureAtlasMaker::sDefaultPaletteBackgroundColor(0U, 0U, 0U);

TextureAtlasMaker::TextureAtlasMaker(std::map<char, ImageData> images)
{
	mImageCollection = images;
}

bool TextureAtlasMaker::CreateTextureAtlas(std::map<char, std::pair<unsigned int, unsigned int>>& texAtlasData, ImageData& texAtlasImage)
{
	if( mImageCollection.size() == 0 )
	{
		return false;
	}

	//Find palette specification

	//Find dimension of each cell
	unsigned int cellWidth;
	unsigned int cellHeight;
	FindCellDimensions(cellWidth, cellHeight);

	//Find number of rows and columns
	unsigned int numberOfColumns;
	unsigned int numberOfRows;
	CalculateGridDimensions(numberOfColumns, numberOfRows);

	//Create palette
	ImageData textureAtlasPalette(static_cast<unsigned int>(cellWidth * numberOfColumns), static_cast<unsigned int>(cellHeight * numberOfRows));
	textureAtlasPalette.Fill(sDefaultPaletteBackgroundColor);

	auto imageIterator = mImageCollection.begin();

	for( int y = 0; y < numberOfRows; y++ )
	{
		for( int x = 0; x < numberOfColumns; x++ )
		{
			if( imageIterator != mImageCollection.end() )
			{
				ImageData::DrawImageOnImageAtPos(x * cellWidth, y * cellHeight, textureAtlasPalette, imageIterator->second);

				texAtlasData.emplace(imageIterator->first, std::make_pair<unsigned int, unsigned int>(x * cellWidth, y * cellHeight));


				imageIterator++;
			}
		}
	}
	texAtlasImage = textureAtlasPalette;

	return true;
}

ImageData TextureAtlasMaker::CreateTextureAtlas(std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>>& ioFontAtlas)
{
	if (ioFontAtlas.size() == 0)
	{
		return ImageData();
	}

	//Find palette specification

	//Find dimension of each cell
	unsigned int cellWidth;
	unsigned int cellHeight;
	FindCellDimensions(cellWidth, cellHeight, ioFontAtlas);

	//Find number of rows and columns
	unsigned int numberOfColumns;
	unsigned int numberOfRows;
	CalculateGridDimensions(numberOfColumns, numberOfRows, ioFontAtlas);

	//Create palette
	ImageData textureAtlasPalette(static_cast<unsigned int>(cellWidth * numberOfColumns), static_cast<unsigned int>(cellHeight * numberOfRows));
	textureAtlasPalette.Fill(sDefaultPaletteBackgroundColor);

	auto imageIterator = ioFontAtlas.begin();

	for (int y = 0; y < numberOfRows; y++)
	{
		for (int x = 0; x < numberOfColumns; x++)
		{
			if (imageIterator != ioFontAtlas.end())
			{
				ImageData::DrawImageOnImageAtPos(x * cellWidth, y * cellHeight, textureAtlasPalette, imageIterator->second.second);

				imageIterator->second.first.atlasX = static_cast<std::uint32_t>(x * cellWidth) + imageIterator->second.first.padding;
				imageIterator->second.first.atlasY = static_cast<std::uint32_t>(y * cellHeight) + imageIterator->second.first.padding;
				
				imageIterator++;
			}
		}
	}

	return textureAtlasPalette;
}

void TextureAtlasMaker::FindCellDimensions(unsigned int& outputWidth, unsigned int& outputHeight)
{
	unsigned int height = 0U;
	unsigned int width = 0U;
	for( auto& it : mImageCollection )
	{
		if( it.second.GetHeight() >= height )
		{
			height = it.second.GetHeight();
		}
		if( it.second.GetWidth() >= width )
		{
			width = it.second.GetWidth();
		}
	}

	outputWidth = width;
	outputHeight = height;
}

void TextureAtlasMaker::FindCellDimensions(unsigned int& outputWidth, unsigned int& outputHeight, std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>>& fontAtlas)
{
	unsigned int height = 0U;
	unsigned int width = 0U;
	for (auto& it : fontAtlas)
	{
		if (it.second.second.GetHeight() >= height)
		{
			height = it.second.second.GetHeight();
		}
		if (it.second.second.GetWidth() >= width)
		{
			width = it.second.second.GetWidth();
		}
	}

	outputWidth = width;
	outputHeight = height;
}

void TextureAtlasMaker::CalculateGridDimensions(unsigned int& outputNumberOfColumns, unsigned int& outputNumberOfRows)
{
	unsigned int cellWidth;
	unsigned int cellHeight;
	FindCellDimensions(cellWidth, cellHeight);

	unsigned int cellsNum = mImageCollection.size();
	int totalSquare = cellWidth * cellHeight * cellsNum;
	float rootSq = sqrtf(totalSquare);

	outputNumberOfColumns = std::ceil(rootSq / cellWidth);
	outputNumberOfRows = std::ceil((float)(cellsNum) / (float)(outputNumberOfColumns));
}

void TextureAtlasMaker::CalculateGridDimensions(unsigned int& outputNumberOfColumns, unsigned int& outputNumberOfRows, std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>>& fontAtlas)
{
	unsigned int cellWidth;
	unsigned int cellHeight;
	FindCellDimensions(cellWidth, cellHeight, fontAtlas);

	unsigned int cellsNum = fontAtlas.size();
	int totalSquare = cellWidth * cellHeight * cellsNum;
	float rootSq = sqrtf(totalSquare);

	outputNumberOfColumns = std::ceil(rootSq / cellWidth);
	outputNumberOfRows = std::ceil((float)(cellsNum) / (float)(outputNumberOfColumns));
}

}// rendering_engine