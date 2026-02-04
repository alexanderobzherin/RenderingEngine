// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
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

struct GlyphMetrics;

/**
 * @class TextureAtlasMaker
 * @brief Builds texture atlases from collections of images.
 *
 * Arranges input images into a grid-based atlas, aiming for a compact,
 * near-square layout. Supports generic image atlases and font glyph atlases.
 */
class TextureAtlasMaker
{
public:
	/**
	 * @brief Constructs an atlas maker from a collection of images.
	 * @param images Map of identifiers to image data.
	 */
	TextureAtlasMaker(std::map<char, ImageData> images);
	~TextureAtlasMaker() = default;

	/**
	 * @brief Creates a texture atlas from the stored image collection.
	 * @param texAtlasData Output map of image identifiers to atlas positions.
	 * @param texAtlasImage Output atlas image.
	 * @return True if the atlas was created successfully.
	 */
	bool CreateTextureAtlas( std::map<char, std::pair<unsigned int, unsigned int>>& texAtlasData, ImageData& texAtlasImage );

	/**
	 * @brief Creates a texture atlas from glyph images and updates glyph metrics.
	 *
	 * Modifies glyph metrics in-place to include atlas coordinates.
	 *
	 * @param ioFontAtlas Map of glyph identifiers to glyph metrics and images.
	 * @return Generated atlas image.
	 */
	static ImageData CreateTextureAtlas(std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>>& ioFontAtlas);

protected:
	/**
	 * @brief Computes maximum cell dimensions for the stored images.
	 * @param outputWidth Output cell width in pixels.
	 * @param outputHeight Output cell height in pixels.
	 */
	void FindCellDimensions(unsigned int& outputWidth, unsigned int& outputHeight);

	/**
	 * @brief Computes maximum cell dimensions for glyph images.
	 * @param outputWidth Output cell width in pixels.
	 * @param outputHeight Output cell height in pixels.
	 * @param fontAtlas Glyph image collection.
	 */
	static void FindCellDimensions(unsigned int& outputWidth, unsigned int& outputHeight, std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>>& fontAtlas);

	/**
	 * @brief Calculates grid dimensions for the stored images.
	 * @param outputNumberOfColumns Output number of columns.
	 * @param outputNumberOfRows Output number of rows.
	 */
	void CalculateGridDimensions(unsigned int& outputNumberOfColumns, unsigned int& outputNumberOfRows);

	/**
	 * @brief Calculates grid dimensions for glyph images.
	 * @param outputNumberOfColumns Output number of columns.
	 * @param outputNumberOfRows Output number of rows.
	 * @param fontAtlas Glyph image collection.
	 */
	static void CalculateGridDimensions(unsigned int& outputNumberOfColumns, unsigned int& outputNumberOfRows, std::unordered_map<std::uint32_t, std::pair<GlyphMetrics, ImageData>>& fontAtlas);

private:
	TextureAtlasMaker( TextureAtlasMaker const& );
	TextureAtlasMaker operator=(TextureAtlasMaker const&);

protected:
	//container of images
	std::map<char, ImageData> mImageCollection;

	static const Color sDefaultPaletteBackgroundColor;
};

} //namespace rendering_engine