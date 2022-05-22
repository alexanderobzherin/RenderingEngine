#include "image_data.hpp"
#include <exception>

namespace rendering_engine
{
ImageData::ImageData()
	:
	mWidth( 0U ),
	mHeight( 0U )
{
}

ImageData::ImageData( unsigned int width, unsigned int height )
	:
	mWidth( width ),
	mHeight( height )
{
	AllocateMemory( width, height );
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

const Color * ImageData::GetPixel( unsigned int x, unsigned int y ) const
{
	if( x < mWidth && y < mHeight )
	{
		return &(mData[x][y]);
	}
	else
	{
		throw("Inapropriate access to data");
	}

	return nullptr;
}

void ImageData::AllocateMemory( unsigned int width, unsigned int height )
{
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