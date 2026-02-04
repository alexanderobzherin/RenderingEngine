// Copyright (c) 2026 Alexander Obzherin � Licensed under the zlib License. See LICENSE.md.

#include "image_data.hpp"
#include "image_codec_jpeg.hpp"
#include "image_codec_png.hpp"
#include <boost/filesystem.hpp>
#include <stdexcept>

namespace rendering_engine
{
ImageData::ImageData()
	:
	ImageData::ImageData(0U, 0U)
{
}

ImageData::ImageData( unsigned int width, unsigned int height )
	:
	mWidth( width ),
	mHeight( height )
{
	AllocateMemory( mWidth, mHeight );
}

ImageData::ImageData(std::string filepath)
	:
	ImageData::ImageData()
{
	auto const pathToTexture = boost::filesystem::path(filepath);
	if( boost::filesystem::exists(pathToTexture) && boost::filesystem::is_regular_file(pathToTexture) )
	{
		boost::filesystem::path const pathToTexture = boost::filesystem::path(filepath);
		
		size_t const dot = pathToTexture.string().find_last_of(".");
		std::string const fileExtension = pathToTexture.string().substr(dot + 1);

		if( std::string{ "jpg" } == fileExtension )
		{
			LoadTextureJpegFile(pathToTexture.string().c_str());
		}
		else
		{
			if( std::string{ "png" } == fileExtension )
			{
				LoadTexturePngFile(pathToTexture.string().c_str());
			}
			else
			{
				throw std::runtime_error("Unsupported texture file format!");
			}
		}
	}
	else
	{
		throw std::runtime_error("Path to texture file is incorrect.");
	}
}

ImageData::ImageData(std::vector<uint8_t> const& fileBytes)
{
	if (fileBytes.size() < 4)
		throw std::runtime_error("Invalid image buffer");

	// PNG signature
	if (fileBytes[0] == 0x89 &&
		fileBytes[1] == 'P' &&
		fileBytes[2] == 'N' &&
		fileBytes[3] == 'G')
	{
		std::vector<std::uint8_t> pixelsRGBA;

		if (!ReadPngFromMemory(fileBytes.data(), fileBytes.size(), mWidth, mHeight, pixelsRGBA))
			throw std::runtime_error("PNG memory decode failed");

		AllocateMemory(mWidth, mHeight);
		Fill(Color(0, 0, 0, 255));
		LoadImageDataRGBA(pixelsRGBA);
		return;
	}

	// JPEG signature: FF D8 FF
	if (fileBytes[0] == 0xFF &&
		fileBytes[1] == 0xD8 &&
		fileBytes[2] == 0xFF)
	{
		std::vector<std::uint8_t> pixelsRGB;

		if (!ReadJpegFromMemory(fileBytes.data(), fileBytes.size(), mWidth, mHeight, pixelsRGB))
			throw std::runtime_error("JPEG memory decode failed");

		AllocateMemory(mWidth, mHeight);
		LoadImageDataRGB(pixelsRGB);

		return;
	}

	throw std::runtime_error("Unsupported image format in memory buffer");
}

ImageData::ImageData(unsigned int width, unsigned int height, std::vector<std::uint8_t> const& pixelsRGBA)
	:
ImageData::ImageData(width, height)
{
	Fill(Color(255, 255, 255, 255));
	LoadImageDataRGBA( pixelsRGBA );
}

ImageData::~ImageData()
{
	CleanAllocatedMemory();
}

ImageData::ImageData( const ImageData & src )
	:
	mWidth( src.GetWidth() ),
	mHeight( src.GetHeight() )
{
	AllocateMemory(src.mWidth, src.mHeight);

	for( unsigned int y = 0; y < mHeight; y++ )
	{
		for( unsigned int x = 0; x < mWidth; x++ )
		{
			mData[x][y] = src.mData[x][y];
		}
	}
}

ImageData & ImageData::operator=( const ImageData & rhs )
{
	if( this == &rhs )
	{
		return *this;
	}
	CleanAllocatedMemory();
	AllocateMemory( rhs.mWidth, rhs.mHeight );

	for( unsigned int y = 0; y < mHeight; y++ )
	{
		for( unsigned int x = 0; x < mWidth; x++ )
		{
			mData[x][y] = rhs.mData[x][y];
		}
	}

	return *this;
}

void ImageData::Fill( Color color )
{
	for( unsigned int y = 0; y < mHeight; y++ )
	{
		for( unsigned int x = 0; x < mWidth; x++ )
		{
			mData[x][y] = color;
		}
	}
}

void ImageData::SetPixel( unsigned int x, unsigned int y, Color color )
{
	if( x < mWidth && y < mHeight )
	{
		mData[x][y] = color;
	}
}

const Color ImageData::GetPixel( unsigned int x, unsigned int y ) const
{
	if( x < mWidth && y < mHeight )
	{
		return (mData[x][y]);
	}
	else
	{
		throw std::runtime_error("Inapropriate access to data");
	}
}

void ImageData::DrawImageOnImageAtPos(unsigned int const x, unsigned int const y, ImageData& toImage, ImageData& fromImage)
{
	//If fromImage can not be placed entirely, it will be cropped on right and bottom side

	//Source image draw from top left pixel

	//At first check if point pos is inside toImage
	if( !(x >= 0 && y >= 0 && x < toImage.GetWidth() && y < toImage.GetHeight()) )
	{
		return;
	}

	//Crop
	int left;
	if( (toImage.GetWidth() - x) < fromImage.GetWidth() )
	{
		left = toImage.GetWidth() - x;
	}
	else
	{
		left = fromImage.GetWidth();
	}

	int bottom;
	if( (toImage.GetHeight() - y) < fromImage.GetHeight() )
	{
		bottom = toImage.GetHeight() - y;
	}
	else
	{
		bottom = fromImage.GetHeight();
	}

	for( int deltaY = 0; deltaY < bottom; deltaY++ )
	{
		for( int deltaX = 0; deltaX < left; deltaX++ )
		{
			toImage.SetPixel(x + deltaX, y + deltaY, fromImage.GetPixel(deltaX, deltaY));
		}
	}
}

std::vector<uint8_t> ImageData::GetImageDataRGBA() const
{
	std::vector<uint8_t> result;
	if(GetHeight() == 0 && GetWidth() == 0)
	{
		return result;
	}

	for( unsigned int y = 0; y < mHeight; y++ )
	{
		for( unsigned int x = 0; x < mWidth; x++ )
		{
			result.push_back(GetPixel(x, y).r);
			result.push_back(GetPixel(x, y).g);
			result.push_back(GetPixel(x, y).b);
			result.push_back(GetPixel(x, y).a);
		}
	}
	return result;
}

std::vector<uint8_t> ImageData::GetImageDataRGB() const
{
	std::vector<uint8_t> result;
	if( GetHeight() == 0 && GetWidth() == 0 )
	{
		return result;
	}

	for( unsigned int y = 0; y < mHeight; y++ )
	{
		for( unsigned int x = 0; x < mWidth; x++ )
		{
			result.push_back(GetPixel(x, y).r);
			result.push_back(GetPixel(x, y).g);
			result.push_back(GetPixel(x, y).b);
		}
	}
	return result;
}

void ImageData::WriteJpegFile(char const* filename)
{
	SaveTextureFileJpeg(*this, filename);
}

bool ImageData::LoadTextureJpegFile(char const* filename)
{
	CleanAllocatedMemory();
	unsigned int width = 0;
	unsigned int height = 0;
	std::vector<std::uint8_t> rgbImageDataVector;

	bool result = ReadJpegFile(filename, width, height, rgbImageDataVector);
	if(result && (rgbImageDataVector.size() == (3U * width * height)))
	{
		mWidth = width;
		mHeight = height;
		AllocateMemory(width, height);
		LoadImageDataRGB(rgbImageDataVector);
	}

	return result;
}

void ImageData::WritePngFile(char const* filename)
{
	SaveTextureFilePng(*this, filename);
}

bool ImageData::LoadTexturePngFile(char const* filename)
{
	CleanAllocatedMemory();
	unsigned int width = 0;
	unsigned int height = 0;
	std::vector<std::uint8_t> rgbaImageDataVector;

	bool result = ReadPngFile(filename, width, height, rgbaImageDataVector);

	if( result && (rgbaImageDataVector.size() == (4U * width * height)) )
	{
		mWidth = width;
		mHeight = height;
		AllocateMemory(width, height);
		LoadImageDataRGBA(rgbaImageDataVector);
	}
	return true;
}

void ImageData::AllocateMemory( unsigned int width, unsigned int height )
{
	mWidth = width;
	mHeight = height;
	mData = new Color*[mWidth];

	for( unsigned int x = 0; x < mWidth; x++ )
	{
		mData[x] = new Color[mHeight];
	}
}

void ImageData::CleanAllocatedMemory()
{
	for( unsigned int x = 0; x < mWidth; x++ )
	{
		if( mData[x] )
		{
			delete[] mData[x];
		}
	}
	if( mData )
	{
		delete[] mData;
	}

	mWidth = 0U;
	mHeight = 0U;
}

void ImageData::LoadImageDataRGBA(std::vector<std::uint8_t> const& pixels)
{
	//Check for data numbers matching
	if( (GetHeight() * GetWidth() * 4) != pixels.size() )
	{
		throw std::runtime_error("Array data size doesn't match image dimension.");
	}
	auto it = pixels.begin();
	for( unsigned int y = 0; y < GetHeight(); y++ )
	{
		for( unsigned int x = 0; x < GetWidth(); x++ )
		{
			uint8_t const r = *it;
			++it;
			uint8_t const g = *it;
			++it;
			uint8_t const b = *it;
			++it;
			uint8_t const a = *it;
			++it;
			Color const color(r, g, b, a);
			SetPixel(x, y, color);
		}
	}
}

void ImageData::LoadImageDataRGB(std::vector<std::uint8_t> const& pixels)
{
	//Check for data numbers matching
	if( (GetHeight() * GetWidth() * 3) != pixels.size() )
	{
		throw std::runtime_error("Array data size doesn't match image dimension.");
	}
	auto it = pixels.begin();
	for( unsigned int y = 0; y < GetHeight(); y++ )
	{
		for( unsigned int x = 0; x < GetWidth(); x++ )
		{
			uint8_t const r = static_cast<uint8_t>(*it);
			++it;
			uint8_t const g = static_cast<uint8_t>(*it);
			++it;
			uint8_t const b = static_cast<uint8_t>(*it);
			++it;

			Color const color(r, g, b);
			SetPixel(x, y, color);
		}
	}
}


} //namespace rendering_engine