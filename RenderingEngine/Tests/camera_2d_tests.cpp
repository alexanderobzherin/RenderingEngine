#include <gtest/gtest.h>

#include "camera_2d.hpp"
#include "i_application.hpp"

#include <glm/glm.hpp>

#include <stdexcept>

using namespace rendering_engine;

namespace
{

constexpr float kTolerance = 0.001f;

// Minimal application stub for Camera2D testing.
// No window, Vulkan renderer, or input controller required.
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
        return {"Camera2DTest", false, 800u, 600u};
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

class Camera2DTest : public ::testing::Test
{
protected:
    TestApplication mApplication;
    Camera2D mCamera{mApplication};

    void SetUp() override
    {
        mCamera.Initialize();
        mCamera.SetViewportSize(800.0f, 600.0f);
    }

    void ExpectPosition(
        const glm::vec2& actual,
        const glm::vec2& expected)
    {
        EXPECT_NEAR(actual.x, expected.x, kTolerance);
        EXPECT_NEAR(actual.y, expected.y, kTolerance);
    }
};

} // namespace

TEST_F(Camera2DTest, ScreenCenterMapsToWorldOrigin)
{
    const auto world =
        mCamera.ScreenToWorld({400.0f, 300.0f});

    ExpectPosition(world, {0.0f, 0.0f});
}

TEST_F(Camera2DTest, TopLeftMapsToNegativeWorldCoordinates)
{
    const auto world =
        mCamera.ScreenToWorld({0.0f, 0.0f});

    ExpectPosition(world, {-400.0f, -300.0f});
}

TEST_F(Camera2DTest, TopRightMapsToExpectedWorldCoordinates)
{
    const auto world =
        mCamera.ScreenToWorld({800.0f, 0.0f});

    ExpectPosition(world, {400.0f, -300.0f});
}

TEST_F(Camera2DTest, BottomLeftMapsToExpectedWorldCoordinates)
{
    const auto world =
        mCamera.ScreenToWorld({0.0f, 600.0f});

    ExpectPosition(world, {-400.0f, 300.0f});
}

TEST_F(Camera2DTest, BottomRightMapsToPositiveWorldCoordinates)
{
    const auto world =
        mCamera.ScreenToWorld({800.0f, 600.0f});

    ExpectPosition(world, {400.0f, 300.0f});
}

TEST_F(Camera2DTest, ArbitraryScreenPositionMapsCorrectly)
{
    const auto world =
        mCamera.ScreenToWorld({200.0f, 150.0f});

    ExpectPosition(world, {-200.0f, -150.0f});
}

TEST_F(Camera2DTest, CameraTranslationAffectsWorldPosition)
{
    mCamera.SetPosition({100.0f, 50.0f});

    const auto world =
        mCamera.ScreenToWorld({400.0f, 300.0f});

    // The current renderer uses the camera's world matrix
    // directly as its view matrix.
    ExpectPosition(world, {-100.0f, -50.0f});
}

TEST_F(Camera2DTest, CameraZoomAffectsWorldPosition)
{
    mCamera.SetZoom(2.0f);

    const auto world =
        mCamera.ScreenToWorld({600.0f, 400.0f});

    ExpectPosition(world, {100.0f, 50.0f});
}

TEST_F(Camera2DTest, CameraRotationAffectsWorldPosition)
{
    mCamera.SetRotation(90.0f);

    const auto world =
        mCamera.ScreenToWorld({500.0f, 300.0f});

    // Inverse of the +90 degree camera view rotation.
    ExpectPosition(world, {0.0f, -100.0f});
}

TEST_F(Camera2DTest, CombinedCameraTransformIsHandled)
{
    mCamera.SetPosition({100.0f, 50.0f});
    mCamera.SetZoom(2.0f);

    const auto world =
        mCamera.ScreenToWorld({600.0f, 400.0f});

    // Inverse of T(100, 50) * S(2).
    ExpectPosition(world, {50.0f, 25.0f});
}

TEST_F(Camera2DTest, RejectsZeroViewportWidth)
{
    mCamera.SetViewportSize(0.0f, 600.0f);

    EXPECT_THROW(
        mCamera.ScreenToWorld({100.0f, 100.0f}),
        std::runtime_error);
}

TEST_F(Camera2DTest, RejectsZeroViewportHeight)
{
    mCamera.SetViewportSize(800.0f, 0.0f);

    EXPECT_THROW(
        mCamera.ScreenToWorld({100.0f, 100.0f}),
        std::runtime_error);
}

TEST_F(Camera2DTest, RejectsNegativeViewportDimensions)
{
    mCamera.SetViewportSize(-800.0f, 600.0f);

    EXPECT_THROW(
        mCamera.ScreenToWorld({100.0f, 100.0f}),
        std::runtime_error);
}

TEST_F(Camera2DTest, RejectsZeroCameraZoom)
{
    mCamera.SetZoom(0.0f);

    EXPECT_THROW(
        mCamera.ScreenToWorld({400.0f, 300.0f}),
        std::runtime_error);
}

TEST_F(Camera2DTest, ResetRestoresDefaultConversion)
{
    mCamera.SetPosition({100.0f, 50.0f});
    mCamera.SetRotation(45.0f);
    mCamera.SetZoom(2.0f);

    mCamera.Reset();

    const auto world =
        mCamera.ScreenToWorld({400.0f, 300.0f});

    ExpectPosition(world, {0.0f, 0.0f});
}
