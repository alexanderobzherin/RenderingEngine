
#include "gtest/gtest.h"

#include "collision_world_2d.hpp"
#include "rectangle_physical_geometry_2d.hpp"
#include "scene_component_2d.hpp"

#include <glm/glm.hpp>

using namespace rendering_engine;

namespace
{

PhysicalProperties2D MakeProperties(
    Mobility mobility = Mobility::Static,
    bool enabled = true)
{
    PhysicalProperties2D properties;
    properties.geometryType =
        PhysicalGeometryType::Rectangle;
    properties.mobility = mobility;
    properties.collisionEnabled = enabled;
    return properties;
}

} // namespace

TEST(CollisionWorld2D, InitiallyEmpty)
{
    CollisionWorld2D world;

    EXPECT_EQ(world.GetStaticCount(), 0u);
    EXPECT_EQ(world.GetMovableCount(), 0u);
    EXPECT_EQ(world.GetTotalCount(), 0u);
    EXPECT_EQ(world.QueryFirstPoint({0, 0}), nullptr);
    EXPECT_TRUE(world.QueryPoint({0, 0}).empty());
}

TEST(CollisionWorld2D, RegistersStaticGeometry)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties());

    CollisionWorld2D world;
    world.Register(&rectangle);

    EXPECT_TRUE(world.IsRegistered(&rectangle));
    EXPECT_EQ(world.GetStaticCount(), 1u);
    EXPECT_EQ(world.GetMovableCount(), 0u);
}

TEST(CollisionWorld2D, RegistersMovableGeometry)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties(Mobility::Movable));

    CollisionWorld2D world;
    world.Register(&rectangle);

    EXPECT_EQ(world.GetStaticCount(), 0u);
    EXPECT_EQ(world.GetMovableCount(), 1u);
}

TEST(CollisionWorld2D, RejectsDisabledGeometry)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties(Mobility::Static, false));

    CollisionWorld2D world;
    world.Register(&rectangle);

    EXPECT_FALSE(world.IsRegistered(&rectangle));
    EXPECT_EQ(world.GetTotalCount(), 0u);
}

TEST(CollisionWorld2D, DuplicateRegistrationIsIgnored)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties());

    CollisionWorld2D world;
    world.Register(&rectangle);
    world.Register(&rectangle);

    EXPECT_EQ(world.GetTotalCount(), 1u);
}

TEST(CollisionWorld2D, NullRegistrationIsIgnored)
{
    CollisionWorld2D world;

    world.Register(nullptr);
    world.Unregister(nullptr);

    EXPECT_FALSE(world.IsRegistered(nullptr));
    EXPECT_EQ(world.GetTotalCount(), 0u);
}

TEST(CollisionWorld2D, PointQueryFindsRectangle)
{
    SceneComponent2D transform;
    transform.SetPosition({100.0f, 50.0f});

    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties(), {20.0f, 10.0f});

    CollisionWorld2D world;
    world.Register(&rectangle);

    auto results = world.QueryPoint({105.0f, 52.0f});

    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].geometry, &rectangle);
    EXPECT_EQ(
        results[0].worldPoint,
        glm::vec2(105.0f, 52.0f));

    EXPECT_EQ(
        world.QueryFirstPoint({105.0f, 52.0f}),
        &rectangle);
}

TEST(CollisionWorld2D, PointQueryMissesRectangle)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties());

    CollisionWorld2D world;
    world.Register(&rectangle);

    EXPECT_TRUE(world.QueryPoint({100, 100}).empty());
    EXPECT_EQ(
        world.QueryFirstPoint({100, 100}),
        nullptr);
}

TEST(CollisionWorld2D, QueryReturnsOverlappingGeometry)
{
    SceneComponent2D firstTransform;
    SceneComponent2D secondTransform;

    RectanglePhysicalGeometry2D first(
        firstTransform, MakeProperties(), {20, 20});

    RectanglePhysicalGeometry2D second(
        secondTransform, MakeProperties(), {10, 10});

    CollisionWorld2D world;
    world.Register(&first);
    world.Register(&second);

    const auto results = world.QueryPoint({0, 0});

    ASSERT_EQ(results.size(), 2u);
    EXPECT_EQ(results[0].geometry, &first);
    EXPECT_EQ(results[1].geometry, &second);
}

TEST(CollisionWorld2D, StaticGeometryIsQueriedFirst)
{
    SceneComponent2D staticTransform;
    SceneComponent2D movableTransform;

    RectanglePhysicalGeometry2D movable(
        movableTransform,
        MakeProperties(Mobility::Movable));

    RectanglePhysicalGeometry2D stationary(
        staticTransform,
        MakeProperties(Mobility::Static));

    CollisionWorld2D world;

    world.Register(&movable);
    world.Register(&stationary);

    EXPECT_EQ(
        world.QueryFirstPoint({0, 0}),
        &stationary);
}

TEST(CollisionWorld2D, MovableGeometryUsesCurrentTransform)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D rectangle(
        transform,
        MakeProperties(Mobility::Movable),
        {10, 10});

    CollisionWorld2D world;
    world.Register(&rectangle);

    EXPECT_EQ(world.QueryFirstPoint({0, 0}), &rectangle);

    transform.SetPosition({100, 50});

    EXPECT_EQ(world.QueryFirstPoint({0, 0}), nullptr);
    EXPECT_EQ(
        world.QueryFirstPoint({100, 50}),
        &rectangle);
}

TEST(CollisionWorld2D, UnregisterRemovesGeometry)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties());

    CollisionWorld2D world;
    world.Register(&rectangle);

    world.Unregister(&rectangle);

    EXPECT_FALSE(world.IsRegistered(&rectangle));
    EXPECT_EQ(world.GetTotalCount(), 0u);
    EXPECT_TRUE(world.QueryPoint({0, 0}).empty());
}

TEST(CollisionWorld2D, UnregisterAllowsReregistration)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties());

    CollisionWorld2D world;

    world.Register(&rectangle);
    world.Unregister(&rectangle);
    world.Register(&rectangle);

    EXPECT_EQ(world.GetTotalCount(), 1u);
    EXPECT_TRUE(world.IsRegistered(&rectangle));
}

TEST(CollisionWorld2D, ClearRemovesAllRegistrations)
{
    SceneComponent2D firstTransform;
    SceneComponent2D secondTransform;

    RectanglePhysicalGeometry2D first(
        firstTransform, MakeProperties());

    RectanglePhysicalGeometry2D second(
        secondTransform,
        MakeProperties(Mobility::Movable));

    CollisionWorld2D world;
    world.Register(&first);
    world.Register(&second);

    ASSERT_EQ(world.GetTotalCount(), 2u);

    world.Clear();

    EXPECT_EQ(world.GetStaticCount(), 0u);
    EXPECT_EQ(world.GetMovableCount(), 0u);
    EXPECT_TRUE(world.QueryPoint({0, 0}).empty());
}

TEST(CollisionWorld2D, UnregisterUnknownGeometryIsSafe)
{
    SceneComponent2D transform;
    RectanglePhysicalGeometry2D rectangle(
        transform, MakeProperties());

    CollisionWorld2D world;

    world.Unregister(&rectangle);

    EXPECT_EQ(world.GetTotalCount(), 0u);
}
