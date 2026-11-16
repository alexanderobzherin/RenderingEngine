
#include "physical_geometry_2d.hpp"
#include "scene_component_2d.hpp"

namespace rendering_engine
{

PhysicalGeometry2D::PhysicalGeometry2D(
    SceneComponent2D& transform,
    PhysicalProperties2D properties)
    : mTransform(transform),
      mProperties(properties)
{
}

PhysicalGeometry2D::~PhysicalGeometry2D() = default;

const PhysicalProperties2D&
PhysicalGeometry2D::GetProperties() const
{
    return mProperties;
}

SceneComponent2D&
PhysicalGeometry2D::GetTransform() const
{
    return mTransform;
}

} // namespace rendering_engine
