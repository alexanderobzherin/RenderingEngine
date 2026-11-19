// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "physical_geometry_definitions.hpp"

#include <glm/glm.hpp>

namespace rendering_engine
{

class SceneComponent2D;
/**
 * @brief Abstract base class for 2D physical geometries.
 *
 * Associates a geometric shape with a scene component's
 * transform and provides an interface for point hit-testing.
 */
class RE_API PhysicalGeometry2D
{
public:
    /**
     * @brief Constructs a physical geometry associated with a scene component.
     *
     * @param transform Scene component providing the geometry's transform.
     * @param properties Physical geometry configuration.
     */
    PhysicalGeometry2D(SceneComponent2D& transform, PhysicalProperties2D properties);
    /**
     * @brief Virtual destructor.
     */
    virtual ~PhysicalGeometry2D();

    PhysicalGeometry2D(const PhysicalGeometry2D&) = delete;
    PhysicalGeometry2D& operator=(const PhysicalGeometry2D&) = delete;
    /**
     * @brief Tests whether a world-space point lies inside the geometry.
     *
     * @param worldPoint Point to test in world coordinates.
     * @return True if the point is contained within the geometry.
     */
    virtual bool ContainsPoint(const glm::vec2& worldPoint) const = 0;
    /**
     * @brief Returns the shape type of the physical geometry.
     * @return Physical geometry type.
     */
    virtual PhysicalGeometryType GetType() const = 0;
    /**
     * @brief Returns the physical geometry configuration.
     * @return Read-only reference to the geometry properties.
     */
    const PhysicalProperties2D& GetProperties() const;
    /**
     * @brief Returns the scene component associated with this geometry.
     * @return Reference to the scene component.
     */
    SceneComponent2D& GetTransform() const;

protected:
    SceneComponent2D& mTransform;
    PhysicalProperties2D mProperties;
};

} // namespace rendering_engine
