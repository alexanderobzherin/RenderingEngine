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
 * @brief Scene-level registry and query system for 2D physical geometry.
 *
 * Stores non-owning pointers to registered geometries.
 * Geometry owners must unregister their geometry before destroying it.
 */
class RE_API CollisionWorld2D
{
public:
    /**
     * @brief Registers a physical geometry for collision queries.
     *
     * Null pointers, disabled geometries, and duplicate registrations
     * are ignored.
     *
     * @param geometry Geometry to register.
     */
    void Register(PhysicalGeometry2D* geometry);
    /**
     * @brief Removes a physical geometry from the registry.
     *
     * Has no effect if the geometry is not registered.
     *
     * @param geometry Geometry to unregister.
     */
    void Unregister(PhysicalGeometry2D* geometry);

    /**
     * @brief Returns all registered geometry containing
     *        the specified world-space point.
     *
     * Results are returned in registration order within
     * each collection, with static geometry first.
     * @param worldPoint Point to test in world coordinates.
     * @return All geometries containing the specified point.
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
     * @param worldPoint Point to test in world coordinates.
     * 
     * @return Pointer to the first matching geometry,
     *         or nullptr if no match is found.
     */
    PhysicalGeometry2D* QueryFirstPoint(const glm::vec2& worldPoint) const;
    /**
     * @brief Checks whether a geometry is registered.
     *
     * @param geometry Geometry to check.
     * @return True if the geometry is registered.
     */
    bool IsRegistered(const PhysicalGeometry2D* geometry) const;
    /**
     * @brief Removes all registered geometries.
     *
     * Does not destroy the geometry objects.
     */
    void Clear();
    /**
     * @brief Returns the number of registered static geometries.
     * @return Static geometry count.
     */
    std::size_t GetStaticCount() const;
    /**
     * @brief Returns the number of registered movable geometries.
     * @return Movable geometry count.
     */
    std::size_t GetMovableCount() const;
    /**
     * @brief Returns the total number of registered geometries.
     * @return Total geometry count.
     */
    std::size_t GetTotalCount() const;

private:
    std::vector<PhysicalGeometry2D*> mStaticGeometry;
    std::vector<PhysicalGeometry2D*> mMovableGeometry;
};

} // namespace rendering_engine
