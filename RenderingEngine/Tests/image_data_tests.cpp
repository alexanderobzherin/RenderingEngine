// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "image_data.hpp"
#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

using namespace rendering_engine;

TEST(Color, ConstructorsAndEqualityIncludeEveryChannel)
{
    const Color black;
    EXPECT_EQ(black.r, 0);
    EXPECT_EQ(black.g, 0);
    EXPECT_EQ(black.b, 0);
    EXPECT_EQ(black.a, 255);
    const Color rgb(12, 34, 56);
    EXPECT_EQ(rgb.r, 12);
    EXPECT_EQ(rgb.g, 34);
    EXPECT_EQ(rgb.b, 56);
    EXPECT_EQ(rgb.a, 255);
    const Color rgba(12, 34, 56, 78);
    EXPECT_EQ(rgba.r, 12);
    EXPECT_EQ(rgba.g, 34);
    EXPECT_EQ(rgba.b, 56);
    EXPECT_EQ(rgba.a, 78);
    EXPECT_TRUE(rgba == Color(12, 34, 56, 78));
    EXPECT_FALSE(rgba == Color(13, 34, 56, 78));
    EXPECT_FALSE(rgba == Color(12, 35, 56, 78));
    EXPECT_FALSE(rgba == Color(12, 34, 57, 78));
    EXPECT_FALSE(rgba == rgb);
}

TEST(ImageData, EmptyImageHasNoPixelsOrBytes)
{
    ImageData image;
    EXPECT_EQ(image.GetWidth(), 0U);
    EXPECT_EQ(image.GetHeight(), 0U);
    EXPECT_EQ(image.GetSizeInBytes(), 0U);
    EXPECT_TRUE(image.GetImageDataRGBA().empty());
    EXPECT_TRUE(image.GetImageDataRGB().empty());
}

TEST(ImageData, SizedImageInitializesPixelsAndFillReplacesAllChannels)
{
    ImageData image(3, 2);
    EXPECT_EQ(image.GetWidth(), 3U);
    EXPECT_EQ(image.GetHeight(), 2U);
    EXPECT_EQ(image.GetSizeInBytes(), 24U);
    for (unsigned y = 0; y < 2; ++y)
        for (unsigned x = 0; x < 3; ++x)
            EXPECT_EQ(image.GetPixel(x, y), Color(0, 0, 0, 255));
    const Color fill(10, 20, 30, 40);
    image.Fill(fill);
    for (unsigned y = 0; y < 2; ++y)
        for (unsigned x = 0; x < 3; ++x)
            EXPECT_EQ(image.GetPixel(x, y), fill);
}

TEST(ImageData, PixelWritesAreIsolatedAndBoundsAreChecked)
{
    ImageData image(3, 2);
    const Color background(1, 2, 3, 4), foreground(5, 6, 7, 8);
    image.Fill(background);
    image.SetPixel(1, 1, foreground);
    for (unsigned y = 0; y < 2; ++y)
        for (unsigned x = 0; x < 3; ++x)
            EXPECT_EQ(image.GetPixel(x, y), (x == 1 && y == 1) ? foreground : background);
    const auto before = image.GetImageDataRGBA();
    image.SetPixel(3, 0, foreground);
    image.SetPixel(0, 2, foreground);
    image.SetPixel(std::numeric_limits<unsigned>::max(), 0, foreground);
    EXPECT_EQ(image.GetImageDataRGBA(), before);
    EXPECT_EQ(image.GetPixel(3, 0), Color());
    EXPECT_EQ(image.GetPixel(0, 2), Color());
}

TEST(ImageData, RawRGBAAndRGBPreserveRowAndChannelOrder)
{
    const std::vector<std::uint8_t> rgba = {1, 2, 3, 4, 5, 6, 7, 8,
                                           9, 10, 11, 12, 13, 14, 15, 16};
    const ImageData image(2, 2, rgba);
    EXPECT_EQ(image.GetWidth(), 2U);
    EXPECT_EQ(image.GetHeight(), 2U);
    EXPECT_EQ(image.GetSizeInBytes(), 16U);
    EXPECT_EQ(image.GetPixel(0, 0), Color(1, 2, 3, 4));
    EXPECT_EQ(image.GetPixel(1, 0), Color(5, 6, 7, 8));
    EXPECT_EQ(image.GetPixel(0, 1), Color(9, 10, 11, 12));
    EXPECT_EQ(image.GetPixel(1, 1), Color(13, 14, 15, 16));
    EXPECT_EQ(image.GetImageDataRGBA(), rgba);
    const std::vector<std::uint8_t> rgb = {1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15};
    EXPECT_EQ(image.GetImageDataRGB(), rgb);
    EXPECT_EQ(image.GetImageDataRGB().size(), 2U * 2U * 3U);
}

TEST(ImageData, RawRGBARejectsBothShortAndLongBuffers)
{
    EXPECT_THROW((ImageData(2, 2, std::vector<std::uint8_t>(15))), std::runtime_error);
    EXPECT_THROW((ImageData(2, 2, std::vector<std::uint8_t>(17))), std::runtime_error);
}

TEST(ImageData, CopyConstructionOwnsIndependentStorage)
{
    ImageData original(2, 3);
    original.Fill(Color(1, 2, 3, 4));
    ImageData copy(original);
    EXPECT_EQ(copy.GetWidth(), 2U);
    EXPECT_EQ(copy.GetHeight(), 3U);
    EXPECT_EQ(copy.GetImageDataRGBA(), original.GetImageDataRGBA());
    copy.SetPixel(1, 2, Color(9, 8, 7, 6));
    EXPECT_EQ(copy.GetPixel(1, 2), Color(9, 8, 7, 6));
    EXPECT_EQ(original.GetPixel(1, 2), Color(1, 2, 3, 4));
}

TEST(ImageData, CopyAssignmentResizesAndOwnsIndependentStorage)
{
    ImageData original(2, 3), copy(1, 1);
    original.Fill(Color(1, 2, 3, 4));
    EXPECT_EQ(&(copy = original), &copy);
    EXPECT_EQ(copy.GetWidth(), 2U);
    EXPECT_EQ(copy.GetHeight(), 3U);
    EXPECT_EQ(copy.GetImageDataRGBA(), original.GetImageDataRGBA());
    copy.SetPixel(1, 2, Color(9, 8, 7, 6));
    EXPECT_EQ(original.GetPixel(1, 2), Color(1, 2, 3, 4));
    const auto before = copy.GetImageDataRGBA();
    const ImageData& same = copy;
    copy = same;
    EXPECT_EQ(copy.GetImageDataRGBA(), before);
}

namespace
{
struct OverlayCase 
{ 
    unsigned x;
    unsigned y; 
    const char* name; 
};

class ImageOverlay : public ::testing::TestWithParam<OverlayCase> 
{
    
};
}

TEST_P(ImageOverlay, CopiesSourcePixelsAndPreservesEverythingOutsideIntersection)
{
    ImageData destination(3, 3);
    const Color background(90, 80, 70, 60);
    destination.Fill(background);
    ImageData source(2, 2, std::vector<std::uint8_t>{
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    const auto sourceBefore = source.GetImageDataRGBA();
    const auto placement = GetParam();
    ImageData::DrawImageOnImageAtPos(placement.x, placement.y, destination, source);
    for (unsigned y = 0; y < 3; ++y)
    {
        for (unsigned x = 0; x < 3; ++x)
        {
            SCOPED_TRACE(::testing::Message() << "pixel " << x << ", " << y);
            const bool covered = x >= placement.x && y >= placement.y &&
                x - placement.x < 2 && y - placement.y < 2;
            EXPECT_EQ(destination.GetPixel(x, y), covered ?
                source.GetPixel(x - placement.x, y - placement.y) : background);
        }
    }
    EXPECT_EQ(source.GetImageDataRGBA(), sourceBefore);
}

INSTANTIATE_TEST_SUITE_P(Placements, ImageOverlay, ::testing::Values(
    OverlayCase{0, 0, "Origin"}, OverlayCase{1, 1, "Offset"},
    OverlayCase{2, 0, "RightClip"}, OverlayCase{0, 2, "BottomClip"},
    OverlayCase{2, 2, "BothClip"}, OverlayCase{3, 0, "OutsideRight"},
    OverlayCase{0, 3, "OutsideBottom"},
    OverlayCase{std::numeric_limits<unsigned>::max(), 0, "FarOutside"}),
    [](const ::testing::TestParamInfo<OverlayCase>& info) { return info.param.name; });

TEST(ImageData, ZeroSizedOverlaysAreNoOps)
{
    ImageData destination(2, 2);
    destination.Fill(Color(1, 2, 3, 4));
    const auto before = destination.GetImageDataRGBA();
    ImageData empty, zeroWidth(0, 2), zeroHeight(2, 0);
    for (ImageData* source : {&empty, &zeroWidth, &zeroHeight})
    {
        ImageData::DrawImageOnImageAtPos(0, 0, destination, *source);
        EXPECT_EQ(destination.GetImageDataRGBA(), before);
        ImageData::DrawImageOnImageAtPos(0, 0, *source, destination);
        EXPECT_TRUE(source->GetImageDataRGBA().empty());
        EXPECT_TRUE(source->GetImageDataRGB().empty());
        EXPECT_EQ(source->GetSizeInBytes(), 0U);
    }
}

