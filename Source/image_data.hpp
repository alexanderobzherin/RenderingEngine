#pragma once

#include <cstdint>

namespace rendering_engine
{
struct Color
{
	Color()
		:
		r( 0U ),
		g( 0U ),
		b( 0U ),
		a( 255U )
	{}
	Color( uint8_t iR, uint8_t iG, uint8_t iB )
		:
		r( iR ),
		g( iG ),
		b( iB )
	{}
	Color( uint8_t iR, uint8_t iG, uint8_t iB, uint8_t iA )
		:
		r( iR ),
		g( iG ),
		b( iB ),
		a( iA )
	{}
	inline bool operator==(Color const& rhs)
	{
		return (this->r == rhs.r) && (this->g == rhs.g) && (this->b == rhs.b) && (this->a == rhs.a);
	}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 255;
};

class ImageData
{
public:
	ImageData();
	ImageData( unsigned int width, unsigned int height );

	~ImageData();

	ImageData( const ImageData& src );
	ImageData& operator=( const ImageData& rhs );

	inline unsigned int GetWidth() const
	{
		return mWidth;
	}
	inline unsigned int GetHeight() const
	{
		return mHeight;
	}

	void Fill( Color color );
	void SetPixel( unsigned int x, unsigned int y, Color color );
	const Color GetPixel( unsigned int x, unsigned int y ) const;

protected:
	//Allocate memory for image data
	void AllocateMemory( unsigned int width, unsigned int height );
	//Clean allocated memory for Color Array Data
	void CleanAllocatedMemory();

private:
	unsigned int mWidth;
	unsigned int mHeight;

	Color** mData;
};

} //namespace rendering_engine