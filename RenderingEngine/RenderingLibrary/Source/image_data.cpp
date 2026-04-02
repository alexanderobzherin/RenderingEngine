// Copyright (c) 2026 Alexander Obzherin � Licensed under the zlib License. See LICENSE.md.

#include "image_data.hpp"
#include "image_codec_jpeg.hpp"
#include "image_codec_png.hpp"
#include "logger.hpp"
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
	mWidth(src.mWidth),
	mHeight(src.mHeight),
	mData(src.mData)
{
}

ImageData & ImageData::operator=( const ImageData & rhs )
{
	if (this == &rhs)
	{
		return *this;
	}

	mWidth = rhs.mWidth;
	mHeight = rhs.mHeight;
	mData = rhs.mData;
	return *this;
}

void ImageData::Fill( Color color )
{
	std::fill(mData.begin(), mData.end(), color);
}

void ImageData::SetPixel( unsigned int x, unsigned int y, Color color )
{
	if (x < mWidth && y < mHeight)
	{
		PixelRef(x, y) = color;
	}
}

const Color ImageData::GetPixel( unsigned int x, unsigned int y ) const
{
	if (x >= mWidth || y >= mHeight)
	{
		LOG_ERROR("Out of bounds access in GetPixel");
		return Color{};
	}

	return PixelRef(x, y);
}

void ImageData::DrawImageOnImageAtPos(unsigned int const x, unsigned int const y, ImageData& toImage, ImageData& fromImage)
{
	// If fromImage can not be placed entirely, it will be cropped on right and bottom side

	// Source image draw from top left pixel

	// At first check if point pos is inside toImage
	if (x >= toImage.GetWidth() || y >= toImage.GetHeight())
	{
		return;
	}

	if (toImage.GetWidth() == 0U || toImage.GetHeight() == 0U ||
		fromImage.GetWidth() == 0U || fromImage.GetHeight() == 0U)
	{
		return;
	}

	// Crop
	unsigned int left;
	if ((toImage.GetWidth() - x) < fromImage.GetWidth())
	{
		left = toImage.GetWidth() - x;
	}
	else
	{
		left = fromImage.GetWidth();
	}

	unsigned int bottom;
	if ((toImage.GetHeight() - y) < fromImage.GetHeight())
	{
		bottom = toImage.GetHeight() - y;
	}
	else
	{
		bottom = fromImage.GetHeight();
	}

	if (left == 0U || bottom == 0U)
	{
		return;
	}

	for (unsigned int deltaY = 0; deltaY < bottom; ++deltaY)
	{
		const size_t srcRowOffset =
			static_cast<size_t>(deltaY) * static_cast<size_t>(fromImage.GetWidth());

		const size_t dstRowOffset =
			static_cast<size_t>(y + deltaY) * static_cast<size_t>(toImage.GetWidth()) +
			static_cast<size_t>(x);

		std::memcpy(
			toImage.mData.data() + dstRowOffset,
			fromImage.mData.data() + srcRowOffset,
			static_cast<size_t>(left) * sizeof(Color));
	}
}

std::vector<uint8_t> ImageData::GetImageDataRGBA() const
{
	if (mWidth == 0 || mHeight == 0)
	{
		return {};
	}

	const size_t totalBytes =
		static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight) * 4;

	std::vector<uint8_t> result(totalBytes);
	std::memcpy(result.data(), mData.data(), totalBytes);

	return result;
}

std::vector<uint8_t> ImageData::GetImageDataRGB() const
{
	if (mWidth == 0 || mHeight == 0)
	{
		return {};
	}

	const size_t pixelCount =
		static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight);

	std::vector<uint8_t> result(pixelCount * 3);

	const Color* src = mData.data();
	uint8_t* dst = result.data();

	for (size_t i = 0; i < pixelCount; ++i)
	{
		dst[i * 3 + 0] = src[i].r;
		dst[i * 3 + 1] = src[i].g;
		dst[i * 3 + 2] = src[i].b;
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
	mData.resize(static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight));
}

void ImageData::CleanAllocatedMemory()
{
	mData.clear();

	mWidth = 0;
	mHeight = 0;
}

void ImageData::LoadImageDataRGBA(const std::vector<uint8_t>& pixels)
{
	const size_t expectedSize =
		static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight) * 4;

	if (pixels.size() != expectedSize)
	{
		throw std::runtime_error("Array data size doesn't match image dimension.");
	}

	mData.resize(mWidth * mHeight);

	std::memcpy(mData.data(), pixels.data(), expectedSize);
}

void ImageData::LoadImageDataRGB(std::vector<std::uint8_t> const& pixels)
{
	const size_t pixelCount =
		static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight);

	const size_t expectedSize = pixelCount * 3;
	if (pixels.size() != expectedSize)
	{
		throw std::runtime_error("Array data size doesn't match image dimension.");
	}

	mData.resize(pixelCount);

	const uint8_t* src = pixels.data();
	Color* dst = mData.data();

	for (size_t i = 0; i < pixelCount; ++i)
	{
		dst[i].r = src[i * 3 + 0];
		dst[i].g = src[i * 3 + 1];
		dst[i].b = src[i * 3 + 2];
		dst[i].a = 255;
	}
}


} //namespace rendering_engine