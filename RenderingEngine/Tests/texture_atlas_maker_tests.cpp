#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"

#include "../RenderingLibrary/Include/image_data.hpp"
#include "../RenderingLibrary/Include/texture_atlas_maker.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

using namespace rendering_engine;

TEST(ImageDataTest, TextureAtlasMaker)
{
    ImageData image1(10U, 10U);
    image1.Fill(Color(255, 0, 0, 255));

    ImageData image2(5U, 5U);
    image2.Fill(Color(0, 255, 0, 255));

    ImageData image3(2U, 2U);
    image3.Fill(Color(0, 0, 255, 255));

    std::map<char, ImageData> imageCollection;

    imageCollection.emplace('A', image1);
    imageCollection.emplace('B', image2);
    imageCollection.emplace('C', image3);

    TextureAtlasMaker texAtlasMaker( imageCollection );

    ImageData atlasImage;
    std::map<char, std::pair<unsigned int, unsigned int>> texAtlasData;

    bool const result = texAtlasMaker.CreateTextureAtlas(texAtlasData, atlasImage);
    if( result )
    {
        atlasImage.WritePngFile("TestTextureAtlas.png");
    }

    EXPECT_TRUE(result);
}