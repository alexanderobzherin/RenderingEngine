#include "image_data.hpp"
#include "image_codec_jpeg.hpp"
#include <stdexcept>

namespace rendering_engine
{
ImageData::ImageData()
	:
	mWidth( 0U ),
	mHeight( 0U )
{
	AllocateMemory( mWidth, mHeight );
}

ImageData::ImageData( unsigned int width, unsigned int height )
	:
	mWidth( width ),
	mHeight( height )
{
	AllocateMemory( mWidth, mHeight );
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

void ImageData::LoadImageData(std::vector<unsigned int> const & pixels)
{
	//Check for data numbers matching
	if((GetHeight() * GetWidth() * 4) != pixels.size())
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

void ImageData::WriteTextureJpegFile(char* filename)
{
	SaveTextureFileJpeg();
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
}

} //namespace rendering_engine