// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "color_space.hpp"
#include "glm_test_helpers.hpp"

using namespace rendering_engine;

TEST(ColorSpace, SRGBEndpointsAndPiecewiseBoundary)
{
    // Reference values evaluated independently in double precision.
    const float inputs[] = {0, 1, 0.04f, 0.04045f, 0.041f, 0.5f};
    const float expected[] = {0, 1, 0.0030959752f, 0.0031308050f, 0.0031742787f, 0.21404114f};
    for (unsigned i = 0; i < 6; ++i)
        EXPECT_NEAR(ColorSpace::SRGBToLinear(inputs[i]), expected[i], 1e-7f) << inputs[i];
}

TEST(ColorSpace, LinearEndpointsAndPiecewiseBoundary)
{
    const float inputs[] = {0, 1, 0.003f, 0.0031308f, 0.0032f, 0.5f};
    const float expected[] = {0, 1, 0.03876f, 0.040449936f, 0.041323359f, 0.73535698f};
    for (unsigned i = 0; i < 6; ++i)
        EXPECT_NEAR(ColorSpace::LinearToSRGB(inputs[i]), expected[i], 2e-7f) << inputs[i];
}

TEST(ColorSpace, RepresentativeValuesRoundTripInBothDirections)
{
    for (float value : {0.0f, 0.1f, 0.25f, 0.5f, 0.75f, 1.0f})
    {
        EXPECT_NEAR(ColorSpace::LinearToSRGB(ColorSpace::SRGBToLinear(value)), value, 2e-6f);
        EXPECT_NEAR(ColorSpace::SRGBToLinear(ColorSpace::LinearToSRGB(value)), value, 2e-6f);
    }
}

TEST(ColorSpace, VectorOverloadsConvertEachChannelIndependently)
{
    const glm::vec3 srgb(0.02f, 0.5f, 1.0f), linear(0.001f, 0.25f, 0.75f);
    test::ExpectNear(ColorSpace::SRGBToLinear(srgb), glm::vec3(
        ColorSpace::SRGBToLinear(srgb.x), ColorSpace::SRGBToLinear(srgb.y),
        ColorSpace::SRGBToLinear(srgb.z)), 1e-7f);
    test::ExpectNear(ColorSpace::LinearToSRGB(linear), glm::vec3(
        ColorSpace::LinearToSRGB(linear.x), ColorSpace::LinearToSRGB(linear.y),
        ColorSpace::LinearToSRGB(linear.z)), 1e-7f);
}
