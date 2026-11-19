// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "physical_geometry_2d.hpp"

namespace rendering_engine
{
/**
 * @brief Rectangular physical geometry for 2D hit-testing.
 *
 * Defines a rectangle centered at the local origin.
 * Hit-testing accounts for the associated scene component's
 * world transform.
 */
class RE_API RectanglePhysicalGeometry2D final : public PhysicalGeometry2D
{
public:
    /**
     * @brief Constructs a rectangular physical geometry.
     *
     * @param transform Scene component providing the world transform.
     * @param properties Physical geometry configuration.
     * @param localSize Rectangle dimensions in local coordinates.
     *
     * @throws std::invalid_argument If either dimension is
     *         non-positive or non-finite.
     */
    RectanglePhysicalGeometry2D(SceneComponent2D& transform, PhysicalProperties2D properties, glm::vec2 localSize = {1.0f, 1.0f});
    /**
     * @brief Tests whether a world-space point lies inside the rectangle.
     *
     * The rectangle's world transform is taken into account.
     * Points on the boundary are considered inside.
     *
     * @param worldPoint Point to test in world coordinates.
     * @return True if the point lies inside or on the rectangle boundary.
     */
    bool ContainsPoint(const glm::vec2& worldPoint) const override;
    /**
     * @brief Returns the rectangle geometry type.
     * @return PhysicalGeometryType::Rectangle.
     */
    PhysicalGeometryType GetType() const override;
    /**
     * @brief Returns the rectangle dimensions in local coordinates.
     * @return Read-only reference to the local size.
     */
    const glm::vec2& GetLocalSize() const;

private:
    glm::vec2 mLocalSize;
};

} // namespace rendering_engine
