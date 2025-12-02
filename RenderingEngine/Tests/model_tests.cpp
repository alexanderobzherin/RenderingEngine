#include <stdio.h>
#include <cstdint>
#include "gtest/gtest.h"

#include "../RenderingLibrary/Include/image_data.hpp"
#include "../RenderingLibrary/Include/model.hpp"
#include "../RenderingLibrary/Include/mesh.hpp"

using ::testing::EmptyTestEventListener;
using ::testing::InitGoogleTest;
using ::testing::Test;
using ::testing::TestEventListeners;
using ::testing::TestInfo;
using ::testing::TestPartResult;
using ::testing::TestSuite;
using ::testing::UnitTest;

using namespace rendering_engine;

TEST(ImageDataTest, ModelsLoading)
{
    std::string const modelFilepath{ "../Content/Models/TestCube/test_cube.fbx" };
    std::string const textureFilepath{ "../Content/Models/TestCube/test_cube_color.png" };

    ImageData textureImageData(textureFilepath);
    textureImageData.GetHeight();

    Model model(modelFilepath);
    ASSERT_TRUE(model.HasMeshes());

    auto const meshName = model.Meshes().at(0)->Name();
    auto const vertices = model.Meshes().at(0)->Vertices();
    auto const indices = model.Meshes().at(0)->Indices();
    auto const normals = model.Meshes().at(0)->Normals();
    auto const texCoord = model.Meshes().at(0)->TextureCoordinates();
    EXPECT_NE(0, vertices.size());
}