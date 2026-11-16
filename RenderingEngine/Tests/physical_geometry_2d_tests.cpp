
#include "gtest/gtest.h"

#include "physical_geometry_definitions.hpp"
#include "physical_geometry_2d.hpp"
#include "physical_geometry_factory_2d.hpp"
#include "rectangle_physical_geometry_2d.hpp"
#include "scene_component_2d.hpp"

#include <glm/glm.hpp>

#include <limits>
#include <memory>
#include <stdexcept>

using namespace rendering_engine;

namespace
{

PhysicalProperties2D EnabledRectangleProperties()
{
    PhysicalProperties2D properties;
    properties.collisionEnabled = true;
    properties.geometryType =
        PhysicalGeometryType::Rectangle;
    return properties;
}

} // namespace

TEST(PhysicalGeometry2D, DefaultProperties)
{
    const PhysicalProperties2D properties;

    EXPECT_FALSE(properties.collisionEnabled);
    EXPECT_EQ(
        properties.geometryType,
        PhysicalGeometryType::Rectangle);
    EXPECT_EQ(properties.mobility, Mobility::Static);
}

TEST(PhysicalGeometry2D, FactoryReturnsNullWhenDisabled)
{
    SceneComponent2D transform;
    PhysicalProperties2D properties;

    auto geometry = PhysicalGeometryFactory2D::Create(
        transform, properties);

    EXPECT_EQ(geometry, nullptr);
}

TEST(PhysicalGeometry2D, FactoryCreatesRectangle)
{
    SceneComponent2D transform;
    const auto properties = EnabledRectangleProperties();

    auto geometry = PhysicalGeometryFactory2D::Create(
        transform, properties);

    ASSERT_NE(geometry, nullptr);

    EXPECT_EQ(
        geometry->GetType(),
        PhysicalGeometryType::Rectangle);

    EXPECT_TRUE(geometry->GetProperties().collisionEnabled);
    EXPECT_EQ(&geometry->GetTransform(), &transform);
}

TEST(PhysicalGeometry2D, FactoryRejectsUnsupportedTypes)
{
    SceneComponent2D transform;
    auto properties = EnabledRectangleProperties();

    properties.geometryType = PhysicalGeometryType::Circle;

    EXPECT_THROW(
        PhysicalGeometryFactory2D::Create(
            transform, properties),
        std::invalid_argument);

    properties.geometryType = PhysicalGeometryType::Polygon;

    EXPECT_THROW(
        PhysicalGeometryFactory2D::Create(
            transform, properties),
        std::invalid_argument);

    properties.geometryType =
        static_cast<PhysicalGeometryType>(999);

    EXPECT_THROW(
        PhysicalGeometryFactory2D::Create(
            transform, properties),
        std::invalid_argument);
}

TEST(PhysicalGeometry2D, FactoryPreservesMobility)
{
    SceneComponent2D transform;
    auto properties = EnabledRectangleProperties();
    properties.mobility = Mobility::Movable;

    auto geometry = PhysicalGeometryFactory2D::Create(
        transform, properties);

    ASSERT_NE(geometry, nullptr);

    EXPECT_EQ(
        geometry->GetProperties().mobility,
        Mobility::Movable);
}

TEST(RectanglePhysicalGeometry2D, DefaultLocalSize)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D geometry(
        transform, EnabledRectangleProperties());

    EXPECT_EQ(geometry.GetLocalSize(), glm::vec2(1.0f));
}

TEST(RectanglePhysicalGeometry2D, ContainsPointAtIdentity)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {2.0f, 4.0f});

    EXPECT_TRUE(geometry.ContainsPoint({0.0f, 0.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({0.5f, 1.0f}));

    EXPECT_FALSE(geometry.ContainsPoint({1.5f, 0.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({0.0f, 2.5f}));
}

TEST(RectanglePhysicalGeometry2D, BoundaryIsIncluded)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {2.0f, 4.0f});

    EXPECT_TRUE(geometry.ContainsPoint({1.0f, 0.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({-1.0f, 0.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({0.0f, 2.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({0.0f, -2.0f}));

    EXPECT_TRUE(geometry.ContainsPoint({1.0f, 2.0f}));
}

TEST(RectanglePhysicalGeometry2D, Translation)
{
    SceneComponent2D transform;
    transform.SetPosition({100.0f, 50.0f});

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {20.0f, 10.0f});

    EXPECT_TRUE(geometry.ContainsPoint({100.0f, 50.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({105.0f, 52.0f}));

    EXPECT_FALSE(geometry.ContainsPoint({0.0f, 0.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({120.0f, 50.0f}));
}

TEST(RectanglePhysicalGeometry2D, Scaling)
{
    SceneComponent2D transform;
    transform.SetScale({2.0f, 3.0f});

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {10.0f, 10.0f});

    // World dimensions: 20 x 30.
    EXPECT_TRUE(geometry.ContainsPoint({9.0f, 14.0f}));

    EXPECT_FALSE(geometry.ContainsPoint({11.0f, 0.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({0.0f, 16.0f}));
}

TEST(RectanglePhysicalGeometry2D, Rotation)
{
    SceneComponent2D transform;
    transform.SetRotation(90.0f);

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {20.0f, 10.0f});

    // Rotated world dimensions: approximately 10 x 20.
    EXPECT_TRUE(geometry.ContainsPoint({0.0f, 9.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({4.0f, 0.0f}));

    EXPECT_FALSE(geometry.ContainsPoint({9.0f, 0.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({0.0f, 12.0f}));
}

TEST(RectanglePhysicalGeometry2D, NegativeScale)
{
    SceneComponent2D transform;
    transform.SetScale({-2.0f, 3.0f});

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {10.0f, 10.0f});

    EXPECT_TRUE(geometry.ContainsPoint({9.0f, 14.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({11.0f, 0.0f}));
}

TEST(RectanglePhysicalGeometry2D, ZeroScale)
{
    SceneComponent2D transform;
    transform.SetScale({0.0f, 1.0f});

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties());

    EXPECT_FALSE(geometry.ContainsPoint({0.0f, 0.0f}));
}

TEST(RectanglePhysicalGeometry2D, TransformChanges)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D geometry(
        transform,
        EnabledRectangleProperties(),
        {10.0f, 10.0f});

    EXPECT_TRUE(geometry.ContainsPoint({0.0f, 0.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({100.0f, 0.0f}));

    transform.SetPosition({100.0f, 0.0f});

    EXPECT_FALSE(geometry.ContainsPoint({0.0f, 0.0f}));
    EXPECT_TRUE(geometry.ContainsPoint({100.0f, 0.0f}));
}

TEST(RectanglePhysicalGeometry2D, ParentTransform)
{
    SceneComponent2D parent;
    SceneComponent2D child;

    child.AttachTo(&parent);
    child.SetPosition({10.0f, 0.0f});

    parent.SetPosition({100.0f, 50.0f});
    parent.SetRotation(90.0f);

    // Refresh the hierarchy explicitly.
    parent.UpdateWorldMatrix();
    child.UpdateWorldMatrix();

    RectanglePhysicalGeometry2D geometry(
        child,
        EnabledRectangleProperties(),
        {10.0f, 10.0f});

    // Child center after parent rotation: (100, 60).
    EXPECT_TRUE(geometry.ContainsPoint({100.0f, 60.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({110.0f, 50.0f}));
}

TEST(RectanglePhysicalGeometry2D, RejectsInvalidDimensions)
{
    SceneComponent2D transform;
    const auto properties = EnabledRectangleProperties();

    EXPECT_THROW(
        RectanglePhysicalGeometry2D(
            transform, properties, {0.0f, 10.0f}),
        std::invalid_argument);

    EXPECT_THROW(
        RectanglePhysicalGeometry2D(
            transform, properties, {-1.0f, 10.0f}),
        std::invalid_argument);

    EXPECT_THROW(
        RectanglePhysicalGeometry2D(
            transform, properties, {10.0f, -1.0f}),
        std::invalid_argument);

    const float nan =
        std::numeric_limits<float>::quiet_NaN();

    EXPECT_THROW(
        RectanglePhysicalGeometry2D(
            transform, properties, {nan, 10.0f}),
        std::invalid_argument);
}

TEST(RectanglePhysicalGeometry2D, RejectsNonFiniteQuery)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D geometry(
        transform, EnabledRectangleProperties());

    const float nan =
        std::numeric_limits<float>::quiet_NaN();

    const float infinity =
        std::numeric_limits<float>::infinity();

    EXPECT_FALSE(geometry.ContainsPoint({nan, 0.0f}));
    EXPECT_FALSE(geometry.ContainsPoint({0.0f, infinity}));
}
