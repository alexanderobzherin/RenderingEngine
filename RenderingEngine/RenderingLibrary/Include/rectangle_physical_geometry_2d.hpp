// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"
#include "physical_geometry_2d.hpp"

namespace rendering_engine
{

class RE_API RectanglePhysicalGeometry2D final : public PhysicalGeometry2D
{
public:
    RectanglePhysicalGeometry2D(SceneComponent2D& transform, PhysicalProperties2D properties, glm::vec2 localSize = {1.0f, 1.0f});

    bool ContainsPoint(const glm::vec2& worldPoint) const override;

    PhysicalGeometryType GetType() const override;

    const glm::vec2& GetLocalSize() const;

private:
    glm::vec2 mLocalSize;
};

} // namespace rendering_engine
