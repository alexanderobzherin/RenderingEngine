// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include "model_material.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace rendering_engine;

TEST(TextureTypeEnum, VisitsEntireInclusiveRangeExactlyOnceAndTerminates)
{
    const Enum<TextureType> range;
    auto iterator = range.begin();
    ASSERT_NE(iterator, range.end());
    EXPECT_EQ(*iterator, TextureType::Begin);
    // Bounded loop turns a broken termination condition into a failure, not a hang.
    for (int value = TextureType::Begin; value <= TextureType::End; ++value)
    {
        ASSERT_NE(iterator, range.end());
        EXPECT_EQ(*iterator, static_cast<TextureType>(value));
        ++iterator;
    }
    EXPECT_EQ(iterator, range.end());
}

TEST(TextureTypeEnum, PrefixAndPostfixHaveIteratorSemantics)
{
    const Enum<TextureType> range;
    auto iterator = range.begin();
    auto copy = iterator;
    EXPECT_EQ(copy, iterator);
    EXPECT_EQ(&(++iterator), &iterator);
    EXPECT_NE(copy, iterator);
    EXPECT_EQ(*iterator, static_cast<TextureType>(static_cast<int>(TextureType::Begin) + 1));
    auto previous = iterator++;
    EXPECT_EQ(*previous, static_cast<TextureType>(static_cast<int>(TextureType::Begin) + 1));
    EXPECT_EQ(*iterator, static_cast<TextureType>(static_cast<int>(TextureType::Begin) + 2));
    EXPECT_EQ(*copy, TextureType::Begin);
}

TEST(TextureTypeEnum, EndDereferenceThrowsAndIncrementStaysAtEnd)
{
    const Enum<TextureType> range;
    auto end = range.end();
    EXPECT_THROW(*end, std::runtime_error);
    ++end;
    EXPECT_EQ(end, range.end());
    EXPECT_EQ(end++, range.end());
    EXPECT_EQ(end, range.end());
    EXPECT_THROW(*end, std::runtime_error);
}
