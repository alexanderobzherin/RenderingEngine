
#include "physical_geometry_factory_2d.hpp"

#include "physical_geometry_2d.hpp"
#include "rectangle_physical_geometry_2d.hpp"

#include <stdexcept>

namespace rendering_engine
{

std::unique_ptr<PhysicalGeometry2D>
PhysicalGeometryFactory2D::Create(
    SceneComponent2D& transform,
    const PhysicalProperties2D& properties,
    const glm::vec2& localSize)
{
    if (!properties.collisionEnabled)
    {
        return nullptr;
    }

    switch (properties.geometryType)
    {
    case PhysicalGeometryType::Rectangle:
        return std::make_unique<RectanglePhysicalGeometry2D>(
            transform, properties, localSize);

    case PhysicalGeometryType::Circle:
    case PhysicalGeometryType::Polygon:
        throw std::invalid_argument(
            "Physical geometry type is not implemented.");
    }

    throw std::invalid_argument(
        "Unknown physical geometry type.");
}

} // namespace rendering_engine
