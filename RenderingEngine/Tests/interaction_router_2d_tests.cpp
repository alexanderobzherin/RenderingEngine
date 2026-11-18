
#include <gtest/gtest.h>

#include "camera_2d.hpp"
#include "i_application.hpp"
#include "interaction_router_2d.hpp"
#include "physical_geometry_definitions.hpp"
#include "rectangle_physical_geometry_2d.hpp"
#include "scene.hpp"
#include "scene_component_2d.hpp"
#include "scene_manager.hpp"

#include <memory>
#include <stdexcept>

using namespace rendering_engine;

namespace
{

class TestApplication final : public IApplication
{
public:
    void Initialize() override {}
    void Run() override {}
    void Update(float) override {}
    void Draw() override {}
    void Shutdown() override {}

    ScreenSettings GetScreenSettings() const override
    {
        return {"InteractionRouter2DTest", false, 800u, 600u};
    }

    FrameMetrics GetFrameMetrics() const override
    {
        return {};
    }

    InputController* GetInputController() override
    {
        return nullptr;
    }
};

PhysicalProperties2D MakeProperties()
{
    PhysicalProperties2D properties;
    properties.collisionEnabled = true;
    properties.geometryType = PhysicalGeometryType::Rectangle;
    properties.mobility = Mobility::Static;
    return properties;
}

struct CallbackState
{
    int pressed = 0;
    int released = 0;
    int cancelled = 0;
    bool lastReleasedInside = false;

    InteractionRouter2D::Callbacks MakeCallbacks()
    {
        return {
            [this]() {
                ++pressed;
            },
            [this](bool inside) {
                ++released;
                lastReleasedInside = inside;
            },
            [this]() {
                ++cancelled;
            }
        };
    }
};

class InteractionRouter2DTest : public ::testing::Test
{
protected:
    TestApplication application;
    SceneManager sceneManager{nullptr, &application};
    Scene scene{sceneManager};
    std::shared_ptr<Camera2D> camera;

    SceneComponent2D firstTransform;
    SceneComponent2D secondTransform;

    RectanglePhysicalGeometry2D first{
        firstTransform, MakeProperties(), {100.0f, 100.0f}
    };

    RectanglePhysicalGeometry2D second{
        secondTransform, MakeProperties(), {100.0f, 100.0f}
    };

    InteractionRouter2D& Router()
    {
        return scene.GetInteractionRouter2D();
    }

    void SetUp() override
    {
        camera = std::make_shared<Camera2D>(application);
        camera->Initialize();
        camera->SetViewportSize(800.0f, 600.0f);

        scene.SetActiveCamera2D(camera);

        scene.GetCollisionWorld2D().Register(&first);
        scene.GetCollisionWorld2D().Register(&second);
    }

    void TearDown() override
    {
        // Clear callbacks and registrations before
        // fixture-owned geometry is destroyed.
        Router().Clear();
        scene.GetCollisionWorld2D().Clear();
        scene.Shutdown();
    }

    void Send(
        InputAction action,
        double x = 400.0,
        double y = 300.0,
        MouseButton button = MouseButton::Left)
    {
        Router().ProcessMouseButtonEvent(
            MouseButtonEvent{button, action, x, y});
    }
};

} // namespace

TEST_F(InteractionRouter2DTest, RejectsNullGeometry)
{
    CallbackState state;

    EXPECT_THROW(
        Router().RegisterTarget(
            nullptr, state.MakeCallbacks()),
        std::invalid_argument);
}

TEST_F(InteractionRouter2DTest, RejectsUnregisteredGeometry)
{
    SceneComponent2D transform;

    RectanglePhysicalGeometry2D unregistered{
        transform, MakeProperties()
    };

    CallbackState state;

    EXPECT_THROW(
        Router().RegisterTarget(
            &unregistered, state.MakeCallbacks()),
        std::invalid_argument);
}

TEST_F(InteractionRouter2DTest, RejectsDuplicateRegistration)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    EXPECT_THROW(
        Router().RegisterTarget(
            &first, state.MakeCallbacks()),
        std::invalid_argument);
}

TEST_F(InteractionRouter2DTest, PressInsideInvokesCallback)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);

    EXPECT_EQ(state.pressed, 1);
    EXPECT_EQ(state.released, 0);
    EXPECT_EQ(state.cancelled, 0);
}

TEST_F(InteractionRouter2DTest, PressOutsideDoesNothing)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press, 700.0, 500.0);

    EXPECT_EQ(state.pressed, 0);
    EXPECT_EQ(state.released, 0);
}

TEST_F(InteractionRouter2DTest, ReleaseInsideReportsTrue)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);
    Send(InputAction::Release);

    EXPECT_EQ(state.pressed, 1);
    EXPECT_EQ(state.released, 1);
    EXPECT_TRUE(state.lastReleasedInside);
}

TEST_F(InteractionRouter2DTest, ReleaseOutsideReportsFalse)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);
    Send(InputAction::Release, 700.0, 500.0);

    EXPECT_EQ(state.pressed, 1);
    EXPECT_EQ(state.released, 1);
    EXPECT_FALSE(state.lastReleasedInside);
}

TEST_F(InteractionRouter2DTest, ReleaseWithoutPressDoesNothing)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Release);

    EXPECT_EQ(state.pressed, 0);
    EXPECT_EQ(state.released, 0);
}

TEST_F(InteractionRouter2DTest, HigherPriorityWins)
{
    CallbackState low;
    CallbackState high;

    Router().RegisterTarget(
        &first, low.MakeCallbacks(), 0);

    Router().RegisterTarget(
        &second, high.MakeCallbacks(), 10);

    Send(InputAction::Press);

    EXPECT_EQ(low.pressed, 0);
    EXPECT_EQ(high.pressed, 1);
}

TEST_F(InteractionRouter2DTest, NewerTargetWinsEqualPriority)
{
    CallbackState older;
    CallbackState newer;

    Router().RegisterTarget(
        &first, older.MakeCallbacks(), 5);

    Router().RegisterTarget(
        &second, newer.MakeCallbacks(), 5);

    Send(InputAction::Press);

    EXPECT_EQ(older.pressed, 0);
    EXPECT_EQ(newer.pressed, 1);
}

TEST_F(InteractionRouter2DTest, CaptureIgnoresRepeatedPress)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);
    Send(InputAction::Press);

    EXPECT_EQ(state.pressed, 1);

    Send(InputAction::Release);

    EXPECT_EQ(state.released, 1);
}

TEST_F(InteractionRouter2DTest, CaptureClearsAfterRelease)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);
    Send(InputAction::Release);

    Send(InputAction::Press);
    Send(InputAction::Release);

    EXPECT_EQ(state.pressed, 2);
    EXPECT_EQ(state.released, 2);
}

TEST_F(InteractionRouter2DTest, CancelInvokesCallback)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);

    Router().CancelAllInteractions();

    EXPECT_EQ(state.cancelled, 1);
    EXPECT_EQ(state.released, 0);

    Send(InputAction::Release);

    EXPECT_EQ(state.released, 0);
}

TEST_F(InteractionRouter2DTest, RepeatedCancelIsSafe)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);

    Router().CancelAllInteractions();
    Router().CancelAllInteractions();

    EXPECT_EQ(state.cancelled, 1);
}

TEST_F(InteractionRouter2DTest, UnregisterCancelsCapture)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);

    Router().UnregisterTarget(&first);

    EXPECT_EQ(state.cancelled, 1);

    Send(InputAction::Release);
    Send(InputAction::Press);

    EXPECT_EQ(state.released, 0);
    EXPECT_EQ(state.pressed, 1);
}

TEST_F(InteractionRouter2DTest, ClearCancelsAndRemovesTargets)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press);

    Router().Clear();

    EXPECT_EQ(state.cancelled, 1);

    Send(InputAction::Press);
    Send(InputAction::Release);

    EXPECT_EQ(state.pressed, 1);
    EXPECT_EQ(state.released, 0);
}

TEST_F(InteractionRouter2DTest, RightMouseButtonIsIgnored)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Press, 400.0, 300.0,
         MouseButton::Right);

    Send(InputAction::Release, 400.0, 300.0,
         MouseButton::Right);

    EXPECT_EQ(state.pressed, 0);
    EXPECT_EQ(state.released, 0);
}

TEST_F(InteractionRouter2DTest, RepeatActionIsIgnored)
{
    CallbackState state;

    Router().RegisterTarget(
        &first, state.MakeCallbacks());

    Send(InputAction::Repeat);

    EXPECT_EQ(state.pressed, 0);
}

TEST_F(InteractionRouter2DTest, CallbackCanUnregisterItself)
{
    int pressed = 0;
    int cancelled = 0;

    InteractionRouter2D::Callbacks callbacks;

    callbacks.onPressed = [&]() {
        ++pressed;
        Router().UnregisterTarget(&first);
    };

    callbacks.onCancelled = [&]() {
        ++cancelled;
    };

    Router().RegisterTarget(&first, callbacks);

    Send(InputAction::Press);

    EXPECT_EQ(pressed, 1);
    EXPECT_EQ(cancelled, 1);

    Send(InputAction::Release);
    EXPECT_EQ(pressed, 1);
}

TEST_F(InteractionRouter2DTest, MissingCameraCancelsCapture)
{
    CallbackState state;

    Router().RegisterTarget(&first, state.MakeCallbacks());

    Send(InputAction::Press);
    ASSERT_EQ(state.pressed, 1);

    scene.SetActiveCamera2D(nullptr);

    Send(InputAction::Release);

    EXPECT_EQ(state.cancelled, 1);
    EXPECT_EQ(state.released, 0);
}
