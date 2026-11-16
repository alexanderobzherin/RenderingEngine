
#include <gtest/gtest.h>

#include "drawable_2d.hpp"
#include "physical_geometry_2d.hpp"
#include "physical_geometry_factory_2d.hpp"
#include "rectangle_physical_geometry_2d.hpp"
#include "scene.hpp"
#include "scene_manager.hpp"
#include "rectangle_2d.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <stdexcept>
#include <utility>

using namespace rendering_engine;

namespace
{

PhysicalProperties2D MakeProperties(
    bool enabled = true,
    Mobility mobility = Mobility::Static)
{
    PhysicalProperties2D properties;
    properties.collisionEnabled = enabled;
    properties.geometryType =
        PhysicalGeometryType::Rectangle;
    properties.mobility = mobility;
    return properties;
}

/**
 * Drawable without GPU resources.
 *
 * Tests the real Drawable2D implementation while avoiding
 * Vulkan and render-resource initialization.
 */
class TestDrawable2D final : public Drawable2D
{
public:
    explicit TestDrawable2D(
        Scene& scene,
        int* destructionCount = nullptr)
        : Drawable2D(RenderResourceContext{}, scene),
        mDestructionCount(destructionCount)
    {
    }

    ~TestDrawable2D() override
    {
        if (mDestructionCount)
        {
            ++(*mDestructionCount);
        }
    }

    void Draw(const Camera2D&) override
    {
    }

private:
    int* mDestructionCount;
};

/**
 * Tracks destruction of physical geometry.
 */
class TrackedGeometry2D final : public PhysicalGeometry2D
{
public:
    TrackedGeometry2D(
        SceneComponent2D& transform,
        int& destructionCount)
        : PhysicalGeometry2D(
              transform, MakeProperties()),
          mDestructionCount(destructionCount)
    {
    }

    ~TrackedGeometry2D() override
    {
        ++mDestructionCount;
    }

    bool ContainsPoint(const glm::vec2&) const override
    {
        return true;
    }

    PhysicalGeometryType GetType() const override
    {
        return PhysicalGeometryType::Rectangle;
    }

private:
    int& mDestructionCount;
};

class Drawable2DPhysicalGeometryTest
    : public ::testing::Test
{
protected:
    // These objects are deliberately constructed without
    // initializing rendering resources.
    SceneManager sceneManager{nullptr, nullptr};
    Scene scene{sceneManager};

    std::unique_ptr<PhysicalGeometry2D> CreateRectangle(
        TestDrawable2D& drawable,
        bool enabled = true,
        Mobility mobility = Mobility::Static)
    {
        return PhysicalGeometryFactory2D::Create(
            drawable.GetTransform(),
            MakeProperties(enabled, mobility),
            {1.0f, 1.0f});
    }
};

} // namespace

namespace rendering_engine
{

template <>
TestDrawable2D* Scene::Spawn<TestDrawable2D, int*>(
    int* destructionCount)
{
    auto* drawable =
        new TestDrawable2D(*this, destructionCount);

    mDrawables2D.push_back(drawable);

    // Intentionally skip Initialize().
    // No GPU resources are needed for these tests.
    return drawable;
}

} // namespace rendering_engine

TEST_F(
    Drawable2DPhysicalGeometryTest,
    InitiallyHasNoGeometry)
{
    TestDrawable2D drawable(scene);

    EXPECT_EQ(drawable.GetPhysicalGeometry(), nullptr);
    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    AttachingGeometryRegistersIt)
{
    TestDrawable2D drawable(scene);

    auto geometry = CreateRectangle(drawable);
    auto* rawGeometry = geometry.get();

    drawable.SetPhysicalGeometry(std::move(geometry));

    EXPECT_EQ(drawable.GetPhysicalGeometry(), rawGeometry);

    EXPECT_TRUE(
        scene.GetCollisionWorld2D().IsRegistered(
            rawGeometry));

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetStaticCount(),
        1u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    MovableGeometryUsesMovableCollection)
{
    TestDrawable2D drawable(scene);

    drawable.SetPhysicalGeometry(
        CreateRectangle(
            drawable, true, Mobility::Movable));

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetStaticCount(),
        0u);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetMovableCount(),
        1u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    DisabledGeometryIsNotRegistered)
{
    TestDrawable2D drawable(scene);

    drawable.SetPhysicalGeometry(
        CreateRectangle(drawable, false));

    EXPECT_EQ(drawable.GetPhysicalGeometry(), nullptr);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    AttachedGeometryUsesDrawableTransform)
{
    TestDrawable2D drawable(scene);

    drawable.SetPhysicalGeometry(
        CreateRectangle(drawable));

    drawable.SetPosition({100.0f, 50.0f});
    drawable.SetScale({20.0f, 10.0f});

    auto hits = scene.QueryPoint2D({105.0f, 52.0f});

    ASSERT_EQ(hits.size(), 1u);

    EXPECT_EQ(
        hits[0].geometry,
        drawable.GetPhysicalGeometry());

    EXPECT_TRUE(
        scene.QueryPoint2D({200.0f, 200.0f}).empty());
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    ReplacingGeometryUnregistersPrevious)
{
    TestDrawable2D drawable(scene);

    int destructionCount = 0;

    auto first = std::make_unique<TrackedGeometry2D>(
        drawable.GetTransform(),
        destructionCount);

    auto* firstPointer = first.get();

    drawable.SetPhysicalGeometry(std::move(first));

    ASSERT_TRUE(
        scene.GetCollisionWorld2D().IsRegistered(
            firstPointer));

    auto second = CreateRectangle(
        drawable, true, Mobility::Movable);

    auto* secondPointer = second.get();

    drawable.SetPhysicalGeometry(std::move(second));

    EXPECT_EQ(destructionCount, 1);

    EXPECT_EQ(
        drawable.GetPhysicalGeometry(),
        secondPointer);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetStaticCount(),
        0u);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetMovableCount(),
        1u);

    EXPECT_TRUE(
        scene.GetCollisionWorld2D().IsRegistered(
            secondPointer));
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    RemovingGeometryUnregistersAndDestroysIt)
{
    TestDrawable2D drawable(scene);

    int destructionCount = 0;

    drawable.SetPhysicalGeometry(
        std::make_unique<TrackedGeometry2D>(
            drawable.GetTransform(),
            destructionCount));

    ASSERT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);

    drawable.SetPhysicalGeometry(nullptr);

    EXPECT_EQ(destructionCount, 1);
    EXPECT_EQ(drawable.GetPhysicalGeometry(), nullptr);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    ShutdownUnregistersAndDestroysGeometry)
{
    TestDrawable2D drawable(scene);

    int destructionCount = 0;

    drawable.SetPhysicalGeometry(
        std::make_unique<TrackedGeometry2D>(
            drawable.GetTransform(),
            destructionCount));

    drawable.Shutdown();

    EXPECT_EQ(destructionCount, 1);
    EXPECT_EQ(drawable.GetPhysicalGeometry(), nullptr);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    DestructorUnregistersGeometry)
{
    int destructionCount = 0;

    {
        TestDrawable2D drawable(scene);

        drawable.SetPhysicalGeometry(
            std::make_unique<TrackedGeometry2D>(
                drawable.GetTransform(),
                destructionCount));

        ASSERT_EQ(
            scene.GetCollisionWorld2D().GetTotalCount(),
            1u);

        // Deliberately do not call Shutdown().
    }

    EXPECT_EQ(destructionCount, 1);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    RejectsGeometryUsingAnotherTransform)
{
    TestDrawable2D drawable(scene);
    SceneComponent2D unrelatedTransform;

    int destructionCount = 0;

    auto invalidGeometry =
        std::make_unique<TrackedGeometry2D>(
            unrelatedTransform,
            destructionCount);

    EXPECT_THROW(
        drawable.SetPhysicalGeometry(
            std::move(invalidGeometry)),
        std::invalid_argument);

    EXPECT_EQ(drawable.GetPhysicalGeometry(), nullptr);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);

    // The rejected unique_ptr argument was destroyed.
    EXPECT_EQ(destructionCount, 1);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    RejectedReplacementPreservesExistingGeometry)
{
    TestDrawable2D drawable(scene);

    drawable.SetPhysicalGeometry(
        CreateRectangle(drawable));

    auto* original = drawable.GetPhysicalGeometry();

    SceneComponent2D unrelatedTransform;

    auto invalidGeometry =
        std::make_unique<
            RectanglePhysicalGeometry2D>(
                unrelatedTransform,
                MakeProperties());

    EXPECT_THROW(
        drawable.SetPhysicalGeometry(
            std::move(invalidGeometry)),
        std::invalid_argument);

    EXPECT_EQ(drawable.GetPhysicalGeometry(), original);

    EXPECT_TRUE(
        scene.GetCollisionWorld2D().IsRegistered(
            original));

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    RepeatedShutdownIsSafe)
{
    TestDrawable2D drawable(scene);

    int destructionCount = 0;

    drawable.SetPhysicalGeometry(
        std::make_unique<TrackedGeometry2D>(
            drawable.GetTransform(),
            destructionCount));

    drawable.Shutdown();
    drawable.Shutdown();

    EXPECT_EQ(destructionCount, 1);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    MultipleDrawablesRegisterIndependently)
{
    TestDrawable2D first(scene);
    TestDrawable2D second(scene);

    first.SetPhysicalGeometry(CreateRectangle(first));
    second.SetPhysicalGeometry(CreateRectangle(second));

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        2u);

    first.Shutdown();

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);

    EXPECT_TRUE(
        scene.GetCollisionWorld2D().IsRegistered(
            second.GetPhysicalGeometry()));

    second.Shutdown();

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}


TEST_F(
    Drawable2DPhysicalGeometryTest,
    SceneDefersDrawableDestructionUntilUpdate)
{
    int destructionCount = 0;

    auto* drawable =
        scene.Spawn<TestDrawable2D>(
            &destructionCount);

    drawable->SetPhysicalGeometry(
        CreateRectangle(*drawable));

    ASSERT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);

    drawable->Destroy();

    // Destroy() should only queue the drawable.
    EXPECT_EQ(destructionCount, 0);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);

    // Scene::Update() processes the pending queue.
    scene.Update(0.0f);

    EXPECT_EQ(destructionCount, 1);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);

    EXPECT_TRUE(
        scene.QueryPoint2D({ 0.0f, 0.0f }).empty());
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    SceneShutdownDestroysRegisteredDrawables)
{
    int destructionCount = 0;

    auto* first =
        scene.Spawn<TestDrawable2D>(
            &destructionCount);

    auto* second =
        scene.Spawn<TestDrawable2D>(
            &destructionCount);

    first->SetPhysicalGeometry(
        CreateRectangle(*first));

    second->SetPhysicalGeometry(
        CreateRectangle(*second));

    ASSERT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        2u);

    scene.Shutdown();

    EXPECT_EQ(destructionCount, 2);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);

    EXPECT_TRUE(
        scene.QueryPoint2D({ 0.0f, 0.0f }).empty());
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    SceneShutdownFlushesPendingDestruction)
{
    int destructionCount = 0;

    auto* drawable =
        scene.Spawn<TestDrawable2D>(
            &destructionCount);

    drawable->SetPhysicalGeometry(
        CreateRectangle(*drawable));

    drawable->Destroy();

    // Shutdown must process pending destruction
    // without requiring another Update().
    scene.Shutdown();

    EXPECT_EQ(destructionCount, 1);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}


TEST_F(
    Drawable2DPhysicalGeometryTest,
    Rectangle2DDefaultPropertiesDisableCollision)
{
    Rectangle2D::Properties properties;
    properties.size = { 80.0f, 40.0f };

    Rectangle2D rectangle(
        RenderResourceContext{},
        scene,
        properties);

    rectangle.InitializePhysicalGeometry();

    EXPECT_EQ(rectangle.GetPhysicalGeometry(), nullptr);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        0u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    Rectangle2DEnabledCollisionCreatesGeometry)
{
    Rectangle2D::Properties properties;
    properties.size = { 80.0f, 40.0f };
    properties.physical.collisionEnabled = true;

    Rectangle2D rectangle(
        RenderResourceContext{},
        scene,
        properties);

    rectangle.InitializePhysicalGeometry();

    ASSERT_NE(rectangle.GetPhysicalGeometry(), nullptr);

    EXPECT_EQ(
        rectangle.GetPhysicalGeometry()->GetType(),
        PhysicalGeometryType::Rectangle);

    EXPECT_EQ(
        scene.GetCollisionWorld2D().GetTotalCount(),
        1u);
}

TEST_F(
    Drawable2DPhysicalGeometryTest,
    Rectangle2DGeometryMatchesConfiguredSize)
{
    Rectangle2D::Properties properties;
    properties.size = { 80.0f, 40.0f };
    properties.physical.collisionEnabled = true;

    Rectangle2D rectangle(
        RenderResourceContext{},
        scene,
        properties);

    rectangle.InitializePhysicalGeometry();

    rectangle.SetPosition({ 100.0f, 50.0f });

    // These assertions assume the geometry uses
    // local coordinates from -0.5 to +0.5.
    EXPECT_EQ(
        scene.QueryPoint2D({ 100.0f, 50.0f }).size(),
        1u);

    EXPECT_EQ(
        scene.QueryPoint2D({ 139.0f, 69.0f }).size(),
        1u);

    EXPECT_TRUE(
        scene.QueryPoint2D({ 141.0f, 50.0f }).empty());

    EXPECT_TRUE(
        scene.QueryPoint2D({ 100.0f, 71.0f }).empty());
}
