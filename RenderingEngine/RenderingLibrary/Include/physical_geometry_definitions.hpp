// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <glm/glm.hpp>

namespace rendering_engine
{
/**
 * @brief Defines the shape of a 2D physical geometry.
 */
enum class PhysicalGeometryType
{
    Rectangle,
    Circle,
    Polygon
};
/**
 * @brief Defines whether a physical geometry can move.
 */
enum class Mobility
{
    Static,
    Movable
};
/**
 * @brief Configuration properties for a 2D physical geometry.
 */
struct PhysicalProperties2D
{
    /// Enables collision detection for the geometry.
    bool collisionEnabled = false;
    /// Shape used for collision detection.
    PhysicalGeometryType geometryType = PhysicalGeometryType::Rectangle;
    /// Defines whether the geometry is static or movable.
    Mobility mobility = Mobility::Static;
};

class PhysicalGeometry2D;
/**
 * @brief Result of a point hit test against 2D physical geometry.
 */
struct PointHitResult2D
{
    /// Geometry that was hit, or nullptr if no hit occurred.
    PhysicalGeometry2D* geometry = nullptr;
    /// Tested point in world coordinates.
    glm::vec2 worldPoint{0.0f};
};

} // namespace rendering_engine
