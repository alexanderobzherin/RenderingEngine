// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{

enum class PhysicalGeometryType
{
    Rectangle,
    Circle,
    Polygon
};

enum class Mobility
{
    Static,
    Movable
};

struct PhysicalProperties2D
{
    bool collisionEnabled = false;
    PhysicalGeometryType geometryType = PhysicalGeometryType::Rectangle;
    Mobility mobility = Mobility::Static;
};

} // namespace rendering_engine
