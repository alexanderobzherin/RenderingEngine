
#include "collision_world_2d.hpp"
#include "physical_geometry_2d.hpp"

#include <algorithm>

namespace rendering_engine
{

void CollisionWorld2D::Register(PhysicalGeometry2D* geometry)
{
    if (!geometry)
    {
        return;
    }

    if (!geometry->GetProperties().collisionEnabled)
    {
        return;
    }

    if (IsRegistered(geometry))
    {
        return;
    }

    if (geometry->GetProperties().mobility == Mobility::Static)
    {
        mStaticGeometry.push_back(geometry);
    }
    else
    {
        mMovableGeometry.push_back(geometry);
    }
}

void CollisionWorld2D::Unregister(PhysicalGeometry2D* geometry)
{
    if (!geometry)
    {
        return;
    }

    const auto removeGeometry = [geometry](std::vector<PhysicalGeometry2D*>& collection)
    {
        collection.erase(
            std::remove(
                collection.begin(),
                collection.end(),
                geometry),
            collection.end());
    };

    removeGeometry(mStaticGeometry);
    removeGeometry(mMovableGeometry);
}

std::vector<PointHitResult2D>
CollisionWorld2D::QueryPoint(const glm::vec2& worldPoint) const
{
    std::vector<PointHitResult2D> results;

    const auto collect =[&results, &worldPoint](const std::vector<PhysicalGeometry2D*>& collection)
    {
        for (PhysicalGeometry2D* geometry : collection)
        {
            if (geometry->ContainsPoint(worldPoint))
            {
                results.push_back({geometry, worldPoint});
            }
        }
    };

    collect(mStaticGeometry);
    collect(mMovableGeometry);

    return results;
}

PhysicalGeometry2D*
CollisionWorld2D::QueryFirstPoint(
    const glm::vec2& worldPoint) const
{
    // Registry order is not visual stacking order.
    // Use QueryPoint() and an appropriate selection policy
    // when multiple overlapping geometries are possible.
    const auto findFirst = [&worldPoint](const std::vector<PhysicalGeometry2D*>& collection)
            -> PhysicalGeometry2D*
    {
        for (PhysicalGeometry2D* geometry : collection)
        {
            if (geometry->ContainsPoint(worldPoint))
            {
                return geometry;
            }
        }

        return nullptr;
    };

    if (auto* geometry = findFirst(mStaticGeometry))
    {
        return geometry;
    }

    return findFirst(mMovableGeometry);
}

bool CollisionWorld2D::IsRegistered(
    const PhysicalGeometry2D* geometry) const
{
    const auto contains = [geometry](const std::vector<PhysicalGeometry2D*>& collection)
    {
        return std::find(
            collection.begin(),
            collection.end(),
            geometry) != collection.end();
    };

    return contains(mStaticGeometry) ||
           contains(mMovableGeometry);
}

void CollisionWorld2D::Clear()
{
    mStaticGeometry.clear();
    mMovableGeometry.clear();
}

std::size_t CollisionWorld2D::GetStaticCount() const
{
    return mStaticGeometry.size();
}

std::size_t CollisionWorld2D::GetMovableCount() const
{
    return mMovableGeometry.size();
}

std::size_t CollisionWorld2D::GetTotalCount() const
{
    return GetStaticCount() + GetMovableCount();
}

} // namespace rendering_engine
