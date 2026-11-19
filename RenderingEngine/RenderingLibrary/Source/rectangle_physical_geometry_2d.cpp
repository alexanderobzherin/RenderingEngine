
#include "rectangle_physical_geometry_2d.hpp"
#include "scene_component_2d.hpp"

#include <glm/gtc/matrix_inverse.hpp>

#include <cmath>
#include <stdexcept>

namespace rendering_engine
{

RectanglePhysicalGeometry2D::RectanglePhysicalGeometry2D( SceneComponent2D& transform, PhysicalProperties2D properties, glm::vec2 localSize)
    : 
    PhysicalGeometry2D(transform, properties),
    mLocalSize(localSize)
{
    if (!std::isfinite(localSize.x) ||
        !std::isfinite(localSize.y) ||
        localSize.x <= 0.0f ||
        localSize.y <= 0.0f)
    {
        throw std::invalid_argument(
            "Rectangle physical geometry size must be "
            "finite and positive.");
    }
}

bool RectanglePhysicalGeometry2D::ContainsPoint(const glm::vec2& worldPoint) const
{
    const glm::mat4& worldMatrix = mTransform.GetWorldMatrix();

    const float determinant = glm::determinant(worldMatrix);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-8f)
    {
        return false;
    }

    const glm::mat4 inverseWorld = glm::inverse(worldMatrix);

    const glm::vec4 localPoint = inverseWorld * glm::vec4(worldPoint.x, worldPoint.y, 0.0f, 1.0f);

    const glm::vec2 halfSize = mLocalSize * 0.5f;

    return std::abs(localPoint.x) <= halfSize.x && std::abs(localPoint.y) <= halfSize.y;
}

PhysicalGeometryType
RectanglePhysicalGeometry2D::GetType() const
{
    return PhysicalGeometryType::Rectangle;
}

const glm::vec2&
RectanglePhysicalGeometry2D::GetLocalSize() const
{
    return mLocalSize;
}

} // namespace rendering_engine
