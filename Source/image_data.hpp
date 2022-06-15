#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

namespace rendering_engine
{
struct Color
{
	Color()
		:
	Color( uint8_t{0U}, uint8_t{0U}, uint8_t{0U}, uint8_t{255U} )
	{}
	Color( uint8_t iR, uint8_t iG, uint8_t iB )
		:
	Color( iR, iG, iB, uint8_t{255U} )
	{}
	Color( uint8_t iR, uint8_t iG, uint8_t iB, uint8_t iA )
		:
		r(std::clamp(iR, uint8_t{0U},  uint8_t{255U})),
		g(std::clamp(iG, uint8_t{0U},  uint8_t{255U})),
		b(std::clamp(iB, uint8_t{0U},  uint8_t{255U})),
		a(std::clamp(iA, uint8_t{0U},  uint8_t{255U}))
	{}
	inline bool operator==(Color const& rhs) const
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


	std::vector<uint8_t> GetImageDataRGBA() const;
	std::vector<uint8_t> GetImageDataRGB() const;

	void WriteTextureJpegFile(char* filename);
	bool LoadTextureJpegFile(char * filename);

protected:
	//Allocate memory for image data
	void AllocateMemory( unsigned int width, unsigned int height );
	//Clean allocated memory for Color Array Data
	void CleanAllocatedMemory();
	void LoadImageDataRGBA(std::vector<unsigned int> const& pixels);
	void LoadImageDataRGB(std::vector<unsigned int> const& pixels);

private:
	unsigned int mWidth;
	unsigned int mHeight;

	Color** mData;
};

} //namespace rendering_engine