// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.


#pragma once

#include "rendering_engine_export.hpp"
#include "physical_geometry_definitions.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace rendering_engine
{

class PhysicalGeometry2D;

/**
 * @brief Scene-level registry and query system for 2D geometry.
 *
 * Stores non-owning pointers to physical geometry.
 *
 * Geometry owners must unregister their geometry before
 * destroying it.
 *
 * The initial implementation performs linear searches.
 * Spatial indexing can be introduced later without
 * changing the public query interface.
 */
class RE_API CollisionWorld2D
{
public:
    void Register(PhysicalGeometry2D* geometry);
    void Unregister(PhysicalGeometry2D* geometry);

    /**
     * @brief Returns all registered geometry containing
     *        the specified world-space point.
     *
     * Results are returned in registration order within
     * each collection, with static geometry first.
     */
    std::vector<PointHitResult2D> QueryPoint(const glm::vec2& worldPoint) const;

    /**
     * @brief Returns the first registered geometry containing
     *        the specified world-space point.
     *
     * Static geometry is searched first, followed by movable
     * geometry. Within each collection, registration order
     * determines which geometry is returned first.
     *
     * @warning This method does not account for rendering
     * order, visual depth, or interaction priority.
     * It must not be used to select the topmost UI element.
     *
     * @return Pointer to the first matching geometry,
     *         or nullptr if no match is found.
     */
    PhysicalGeometry2D* QueryFirstPoint(const glm::vec2& worldPoint) const;

    bool IsRegistered(const PhysicalGeometry2D* geometry) const;

    void Clear();

    std::size_t GetStaticCount() const;
    std::size_t GetMovableCount() const;
    std::size_t GetTotalCount() const;

private:
    std::vector<PhysicalGeometry2D*> mStaticGeometry;
    std::vector<PhysicalGeometry2D*> mMovableGeometry;
};

} // namespace rendering_engine
