// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "physical_geometry_definitions.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace rendering_engine
{

class PhysicalGeometry2D;
class SceneComponent2D;

class RE_API PhysicalGeometryFactory2D
{
public:
    static std::unique_ptr<PhysicalGeometry2D> Create(
        SceneComponent2D& transform,
        const PhysicalProperties2D& properties,
        const glm::vec2& localSize = {1.0f, 1.0f});
};

} // namespace rendering_engine
