// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "texture_atlas_maker.hpp"
#include <gtest/gtest.h>

using namespace rendering_engine;

namespace
{
using Positions = std::map<char, std::pair<unsigned int, unsigned int>>;

ImageData SolidImage(unsigned width, unsigned height, Color color)
{
    ImageData image(width, height);
    image.Fill(color);
    return image;
}

void ExpectAtlasPixels(const ImageData& atlas, const std::map<char, ImageData>& sources,
                       const Positions& positions)
{
    ASSERT_EQ(positions.size(), sources.size());
    // Account for every pixel, including cell padding and unused cells.
    for (unsigned y = 0; y < atlas.GetHeight(); ++y)
    {
        for (unsigned x = 0; x < atlas.GetWidth(); ++x)
        {
            SCOPED_TRACE(::testing::Message() << "pixel " << x << ", " << y);
            Color expected(0, 0, 0, 255);
            unsigned coveringImages = 0;
            for (const auto& source : sources)
            {
                const auto found = positions.find(source.first);
                ASSERT_NE(found, positions.end());
                const auto left = found->second.first;
                const auto top = found->second.second;
                if (x >= left && y >= top && x - left < source.second.GetWidth() &&
                    y - top < source.second.GetHeight())
                {
                    ++coveringImages;
                    expected = source.second.GetPixel(x - left, y - top);
                }
            }
            EXPECT_LE(coveringImages, 1U);
            EXPECT_EQ(atlas.GetPixel(x, y), expected);
        }
    }
}
}

TEST(TextureAtlasMaker, EmptyCollectionReportsFailure)
{
    TextureAtlasMaker maker({});
    Positions positions;
    ImageData atlas;
    EXPECT_FALSE(maker.CreateTextureAtlas(positions, atlas));
    EXPECT_TRUE(positions.empty());
    EXPECT_EQ(atlas.GetWidth(), 0U);
    EXPECT_EQ(atlas.GetHeight(), 0U);
    EXPECT_TRUE(atlas.GetImageDataRGBA().empty());
}

TEST(TextureAtlasMaker, SingleImagePreservesPixelsAndRecordsOrigin)
{
    ImageData source(2, 2, std::vector<std::uint8_t>{
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    const std::map<char, ImageData> images{{'A', source}};
    TextureAtlasMaker maker(images);
    Positions positions;
    ImageData atlas;
    ASSERT_TRUE(maker.CreateTextureAtlas(positions, atlas));
    EXPECT_EQ(atlas.GetWidth(), 2U);
    EXPECT_EQ(atlas.GetHeight(), 2U);
    const Positions expected{{'A', {0, 0}}};
    EXPECT_EQ(positions, expected);
    EXPECT_EQ(atlas.GetImageDataRGBA(), source.GetImageDataRGBA());
}

TEST(TextureAtlasMaker, UniformImagesUseGridAndLeaveUnusedCellBlack)
{
    const std::map<char, ImageData> images{
        {'C', SolidImage(2, 2, Color(0, 0, 255, 64))},
        {'A', SolidImage(2, 2, Color(255, 0, 0, 128))},
        {'B', SolidImage(2, 2, Color(0, 255, 0, 192))}
    };
    TextureAtlasMaker maker(images);
    Positions positions;
    ImageData atlas;
    ASSERT_TRUE(maker.CreateTextureAtlas(positions, atlas));
    ASSERT_EQ(atlas.GetWidth(), 4U);
    ASSERT_EQ(atlas.GetHeight(), 4U);
    // This overload uses an ordered map, unlike the optional glyph overload.
    const Positions expected{{'A', {0, 0}}, {'B', {2, 0}}, {'C', {0, 2}}};
    EXPECT_EQ(positions, expected);
    ExpectAtlasPixels(atlas, images, positions);
}

TEST(TextureAtlasMaker, MixedSizesUseMaximumCellDimensionsAndBlackPadding)
{
    const std::map<char, ImageData> images{
        {'A', SolidImage(2, 3, Color(255, 0, 0))},
        {'B', SolidImage(4, 1, Color(0, 255, 0))},
        {'C', SolidImage(1, 2, Color(0, 0, 255))}
    };
    TextureAtlasMaker maker(images);
    Positions positions;
    ImageData atlas;
    ASSERT_TRUE(maker.CreateTextureAtlas(positions, atlas));
    // 4x3 cells, two columns and two rows for these three images.
    ASSERT_EQ(atlas.GetWidth(), 8U);
    ASSERT_EQ(atlas.GetHeight(), 6U);
    const Positions expected{{'A', {0, 0}}, {'B', {4, 0}}, {'C', {0, 3}}};
    EXPECT_EQ(positions, expected);
    ExpectAtlasPixels(atlas, images, positions);
}
