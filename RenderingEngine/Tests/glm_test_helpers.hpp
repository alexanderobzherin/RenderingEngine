// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <gtest/gtest.h>
#include <glm/glm.hpp>

namespace rendering_engine::test
{
template<glm::length_t N, typename T, glm::qualifier Q>
void ExpectNear(const glm::vec<N, T, Q>& actual, const glm::vec<N, T, Q>& expected,
                float tolerance = 1e-5f)
{
    for (glm::length_t i = 0; i < N; ++i)
    {
        EXPECT_NEAR(actual[i], expected[i], tolerance) << "component " << i;
    }
}

inline void ExpectNear(const glm::mat4& actual, const glm::mat4& expected,
                       float tolerance = 1e-5f)
{
    for (glm::length_t column = 0; column < 4; ++column)
    {
        SCOPED_TRACE(::testing::Message() << "column " << column);
        ExpectNear(actual[column], expected[column], tolerance);
    }
}
} // namespace rendering_engine::test
