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

class RE_API PhysicalGeometry2D
{
public:
    PhysicalGeometry2D(SceneComponent2D& transform, PhysicalProperties2D properties);

    virtual ~PhysicalGeometry2D();

    PhysicalGeometry2D(const PhysicalGeometry2D&) = delete;
    PhysicalGeometry2D& operator=(const PhysicalGeometry2D&) = delete;

    virtual bool ContainsPoint(const glm::vec2& worldPoint) const = 0;
    
    virtual PhysicalGeometryType GetType() const = 0;

    const PhysicalProperties2D& GetProperties() const;

    SceneComponent2D& GetTransform() const;

protected:
    SceneComponent2D& mTransform;
    PhysicalProperties2D mProperties;
};

} // namespace rendering_engine
