// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "material_types.hpp"
#include "glm_test_helpers.hpp"
#include <algorithm>
#include <cstring>

using namespace rendering_engine;
using rendering_engine::test::ExpectNear;

namespace
{
using Type = MaterialParameterLayoutEntry::Type;

PackedMaterialData MakeMaterial()
{
    // Gaps and sentinel bytes make writes outside a parameter observable.
    return {std::vector<std::uint8_t>(96, 0xA5), {
        {"Scalar", 4, sizeof(float), Type::Float},
        {"Pair", 16, sizeof(glm::vec2), Type::Vec2},
        {"Triple", 32, sizeof(glm::vec3), Type::Vec3},
        {"Color", 64, sizeof(glm::vec4), Type::Vec4}
    }};
}

template<typename T>
T ReadValue(const PackedMaterialData& material, std::size_t offset)
{
    T value{};
    std::memcpy(&value, material.buffer.data() + offset, sizeof(T));
    return value;
}

void ExpectUnchangedOutside(const PackedMaterialData& material,
                            const std::vector<std::uint8_t>& before,
                            std::size_t offset, std::size_t size)
{
    ASSERT_EQ(material.buffer.size(), before.size());
    for (std::size_t i = 0; i < before.size(); ++i)
    {
        if (i < offset || i >= offset + size)
        {
            EXPECT_EQ(material.buffer[i], before[i]) << "byte " << i;
        }
    }
}

void ExpectLayout(const std::vector<MaterialParameterLayoutEntry>& actual,
                  const std::vector<MaterialParameterLayoutEntry>& expected)
{
    ASSERT_EQ(actual.size(), expected.size());
    for (const auto& entry : expected)
    {
        SCOPED_TRACE(entry.name);
        // Names and metadata form the contract; vector ordering need not.
        const auto found = std::find_if(actual.begin(), actual.end(),
            [&](const MaterialParameterLayoutEntry& candidate) { return candidate.name == entry.name; });
        ASSERT_NE(found, actual.end());
        EXPECT_EQ(found->type, entry.type);
        EXPECT_EQ(found->offset, entry.offset);
        EXPECT_EQ(found->size, entry.size);
    }
}
}

TEST(PackedMaterialData, FloatWritesOnlyItsLayoutSlot)
{
    auto material = MakeMaterial();
    const auto before = material.buffer;
    material.SetMaterialFloat("Scalar", 1.25f);
    EXPECT_FLOAT_EQ(ReadValue<float>(material, 4), 1.25f);
    ExpectUnchangedOutside(material, before, 4, sizeof(float));
}

TEST(PackedMaterialData, Vec2WritesOnlyItsLayoutSlot)
{
    auto material = MakeMaterial();
    const auto before = material.buffer;
    material.SetMaterialVec2("Pair", glm::vec2(1.25f, -2.5f));
    ExpectNear(ReadValue<glm::vec2>(material, 16), glm::vec2(1.25f, -2.5f));
    ExpectUnchangedOutside(material, before, 16, sizeof(glm::vec2));
}

TEST(PackedMaterialData, Vec3WritesOnlyItsLayoutSlot)
{
    auto material = MakeMaterial();
    const auto before = material.buffer;
    material.SetMaterialVec3("Triple", glm::vec3(1.25f, -2.5f, 3.75f));
    ExpectNear(ReadValue<glm::vec3>(material, 32), glm::vec3(1.25f, -2.5f, 3.75f));
    ExpectUnchangedOutside(material, before, 32, sizeof(glm::vec3));
}

TEST(PackedMaterialData, Vec4WritesOnlyItsLayoutSlot)
{
    auto material = MakeMaterial();
    const auto before = material.buffer;
    material.SetMaterialVec4("Color", glm::vec4(1.25f, -2.5f, 3.75f, 0.5f));
    ExpectNear(ReadValue<glm::vec4>(material, 64), glm::vec4(1.25f, -2.5f, 3.75f, 0.5f));
    ExpectUnchangedOutside(material, before, 64, sizeof(glm::vec4));
}

TEST(PackedMaterialData, UnknownNamesAndWrongTypesLeaveBufferUnchanged)
{
    auto material = MakeMaterial();
    const auto before = material.buffer;
    material.SetMaterialFloat("Missing", 1);
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialVec2("Missing", glm::vec2(2));
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialVec3("Missing", glm::vec3(3));
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialVec4("Missing", glm::vec4(4));
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialFloat("Color", 1);
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialVec2("Triple", glm::vec2(2));
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialVec3("Pair", glm::vec3(3));
    EXPECT_EQ(material.buffer, before);
    material.SetMaterialVec4("Scalar", glm::vec4(4));
    EXPECT_EQ(material.buffer, before);
}

TEST(MaterialLayouts, Font2DProtectsShaderPackingContract)
{
    ExpectLayout(Font2DLayout, {
        {"FontColor", 0, 16, Type::Vec4},
        {"OutlineColor", 16, 16, Type::Vec4},
        {"OutlineThicknessPx", 32, 4, Type::Float},
        {"InvAtlasSizeWidth", 36, 4, Type::Float},
        {"InvAtlasSizeHeight", 40, 4, Type::Float}
    });
}

TEST(MaterialLayouts, Rectangle2DProtectsShaderPackingContract)
{
    ExpectLayout(Rectangle2DLayout, {{"Color", 0, 16, Type::Vec4}});
}
