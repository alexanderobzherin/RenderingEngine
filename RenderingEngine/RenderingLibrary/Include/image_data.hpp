// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <algorithm>

#include "rendering_engine_export.hpp"

namespace rendering_engine
{

/**
* @struct Color
* @brief Represents a color with red, green, blue, and alpha channels.
*/
struct Color
{
	/**
	 * @brief Default constructor. Initializes to black with full opacity.
	 */
	Color()
		:
	Color( uint8_t{0U}, uint8_t{0U}, uint8_t{0U}, uint8_t{255U} )
	{}
	/**
	 * @brief Constructs a fully opaque RGB color.
	 * @param iR Red channel [0�255]
	 * @param iG Green channel [0�255]
	 * @param iB Blue channel [0�255]
	 */
	Color( uint8_t iR, uint8_t iG, uint8_t iB )
		:
	Color( iR, iG, iB, uint8_t{255U} )
	{}
	/**
	 * @brief Constructs a color with specified RGBA channels.
	 * @param iR Red channel [0�255]
	 * @param iG Green channel [0�255]
	 * @param iB Blue channel [0�255]
	 * @param iA Alpha channel [0�255]
	 */
	Color( uint8_t iR, uint8_t iG, uint8_t iB, uint8_t iA )
		:
		r(std::clamp(iR, uint8_t{0U},  uint8_t{255U})),
		g(std::clamp(iG, uint8_t{0U},  uint8_t{255U})),
		b(std::clamp(iB, uint8_t{0U},  uint8_t{255U})),
		a(std::clamp(iA, uint8_t{0U},  uint8_t{255U}))
	{}

	/**
	 * @brief Equality operator for comparing two colors.
	 */
	inline bool operator==(Color const& rhs) const
	{
		return (this->r == rhs.r) && (this->g == rhs.g) && (this->b == rhs.b) && (this->a == rhs.a);
	}

	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 255;
};

/**
 * @class ImageData
 * @brief Represents raw 2D image data stored in memory.
 */
class RE_API ImageData
{
public:
	/**
	 * @brief Constructs an empty image.
	 */
	ImageData();

	/**
	 * @brief Constructs a blank image with specified dimensions.
	 * @param width Image width in pixels
	 * @param height Image height in pixels
	 */
	ImageData(unsigned int width, unsigned int height);

	/**
	 * @brief Loads image data from a file.
	 * @param filepath Path to the image file (JPG or PNG)
	 */
	ImageData(std::string filepath);

	/**
	 * @brief Constructs an image by decoding raw file bytes.
	 *
	 * This constructor allows loading an image from a memory buffer rather than a file
	 * on disk. The data is interpreted as either PNG or JPEG depending on format
	 * detection. Decoded pixel data is stored internally in 32-bit RGBA format.
	 *
	 * @param fileBytes Raw contents of an image file (PNG or JPEG).
	 */
	ImageData(std::vector<uint8_t> const& fileBytes);

	/**
	 * @brief Constructs image from raw RGBA pixel buffer.
	 * @param width Image width
	 * @param height Image height
	 * @param pixelsRGBA Vector of RGBA pixel data (4 bytes per pixel)
	 */
	ImageData(unsigned int width, unsigned int height, std::vector<std::uint8_t> const& pixelsRGBA);

	/**
	 * @brief Destructor that frees memory.
	 */
	~ImageData();

	/**
	 * @brief Copy constructor.
	 */
	ImageData(const ImageData& src);

	/**
	 * @brief Copy assignment operator.
	 */
	ImageData& operator=(const ImageData& rhs);

	/**
	 * @brief Returns the image width.
	 */
	inline unsigned int GetWidth() const
	{
		return mWidth;
	}

	/**
	 * @brief Returns the image height.
	 */
	inline unsigned int GetHeight() const
	{
		return mHeight;
	}

	/**
	 * @brief Fills the image with a solid color.
	 * @param color The color to fill with
	 */
	void Fill( Color color );

	/**
	 * @brief Sets the color of a specific pixel.
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @param color The color to set
	 */
	void SetPixel( unsigned int x, unsigned int y, Color color );

	/**
	 * @brief Retrieves the color of a specific pixel.
	 * @param x X coordinate
	 * @param y Y coordinate
	 * @return The color of the pixel
	 */
	const Color GetPixel( unsigned int x, unsigned int y ) const;

	/**
	 * @brief Overlays one image on top of another at a given position.
	 * @param x Top-left X coordinate for the overlay
	 * @param y Top-left Y coordinate for the overlay
	 * @param toImage Destination image
	 * @param fromImage Source image
	 */
	static void DrawImageOnImageAtPos(unsigned int const x, unsigned int const y, ImageData& toImage, ImageData& fromImage);

	/**
	 * @brief Gets raw image data in RGBA format.
	 * @return A vector containing 4 bytes per pixel (RGBA)
	 */
	std::vector<uint8_t> GetImageDataRGBA() const;

	/**
	 * @brief Gets raw image data in RGB format.
	 * @return A vector containing 3 bytes per pixel (RGB)
	 */
	std::vector<uint8_t> GetImageDataRGB() const;

	/**
	 * @brief Gets the total size of the image in memory.
	 * @return Size in bytes
	 */
	size_t GetSizeInBytes() const
	{
		return static_cast<size_t>(mWidth) * static_cast<size_t>(mHeight) * sizeof(Color);
	}

	/**
	 * @brief Writes the image data to a JPEG file.
	 * @param filename Path to output file
	 */
	void WriteJpegFile(char const* filename);


	/**
	 * @brief Writes the image data to a PNG file.
	 * @param filename Path to output file
	 */
	void WritePngFile(char const* filename);

protected:
	/**
	 * @brief Allocates memory for internal pixel storage.
	 * @param width Image width
	 * @param height Image height
	 */
	void AllocateMemory( unsigned int width, unsigned int height );

	/**
	 * @brief Frees allocated memory.
	 */
	void CleanAllocatedMemory();
	/**
	 * @brief Loads image data from a 32-bit RGBA buffer.
	 */
	void LoadImageDataRGBA(std::vector<std::uint8_t> const& pixels);
	/**
	 * @brief Loads image data from a 24-bit RGB buffer.
	 */
	void LoadImageDataRGB(std::vector<std::uint8_t> const& pixels);

	/**
	 * @brief Loads image from a PNG file.
	 */
	bool LoadTexturePngFile(char const* filename);

	/**
	 * @brief Loads image from a JPEG file.
	 */
	bool LoadTextureJpegFile(char const* filename);

private:
	unsigned int mWidth;
	unsigned int mHeight;

	Color** mData;
};

} //namespace rendering_engine