
#include "interaction_router_2d.hpp"

#include "camera_2d.hpp"
#include "collision_world_2d.hpp"
#include "physical_geometry_2d.hpp"
#include "scene.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace rendering_engine
{

InteractionRouter2D::InteractionRouter2D(Scene& scene)
    : mScene(scene)
{
}

void InteractionRouter2D::RegisterTarget(
    PhysicalGeometry2D* geometry,
    Callbacks callbacks,
    int priority)
{
    if (!geometry)
    {
        throw std::invalid_argument(
            "InteractionRouter2D: null geometry");
    }

    if (!mScene.GetCollisionWorld2D().IsRegistered(geometry))
    {
        throw std::invalid_argument(
            "InteractionRouter2D: geometry is not registered "
            "in the scene collision world");
    }

    if (FindRegistration(geometry))
    {
        throw std::invalid_argument(
            "InteractionRouter2D: target already registered");
    }

    mTargets[geometry] = {geometry, std::move(callbacks), priority, mNextOrder++};
}

void InteractionRouter2D::UnregisterTarget(PhysicalGeometry2D* geometry)
{
    if (!geometry)
    {
        return;
    }

    if (mCapturedGeometry == geometry)
    {
        CancelAllInteractions();
    }

    auto found = mTargets.find(geometry);
    if (found != mTargets.end())
    {
        mTargets.erase(found);
    }
}

InteractionRouter2D::Target*
InteractionRouter2D::FindRegistration(PhysicalGeometry2D* geometry)
{
    auto found = mTargets.find(geometry);

    return found != mTargets.end() ? &found->second : nullptr;
}

const InteractionRouter2D::Target*
InteractionRouter2D::FindRegistration(PhysicalGeometry2D* geometry) const
{
    auto found = mTargets.find(geometry);
    
    return found != mTargets.end() ? &found->second : nullptr;
}

PhysicalGeometry2D*
InteractionRouter2D::FindTarget(const glm::vec2& worldPosition) const
{
    const Target* selected = nullptr;

    for (const auto& [geometry, target] : mTargets)
    {
        if (!geometry->ContainsPoint(worldPosition))
        {
            continue;
        }

        if (!selected ||
            target.priority > selected->priority ||
            (target.priority == selected->priority &&
                target.order > selected->order))
        {
            selected = &target;
        }
    }

    return selected ? selected->geometry : nullptr;
}

void InteractionRouter2D::ProcessMouseButtonEvent(const MouseButtonEvent& event)
{
    // C2 supports the left mouse button only.
    if (event.button != MouseButton::Left)
    {
        return;
    }

    if (event.action == InputAction::Repeat)
    {
        return;
    }

    const auto camera = mScene.GetActiveCamera2D();

    if (!camera)
    {
        CancelAllInteractions();
        return;
    }

    const glm::vec2 screenPosition{
        static_cast<float>(event.x),
        static_cast<float>(event.y)
    };

    const glm::vec2 worldPosition =
        camera->ScreenToWorld(screenPosition);

    if (event.action == InputAction::Press)
    {
        // Do not start another interaction while captured.
        if (mCapturedGeometry)
        {
            return;
        }

        mCapturedGeometry = FindTarget(worldPosition);

        if (auto* target = FindRegistration(mCapturedGeometry))
        {
            // Copy before invoking user code: callbacks may
            // modify target registrations.
            auto callback = target->callbacks.onPressed;

            if (callback)
            {
                callback();
            }
        }
    }
    else if (event.action == InputAction::Release)
    {
        PhysicalGeometry2D* captured = mCapturedGeometry;

        // Clear capture before invoking user code.
        mCapturedGeometry = nullptr;

        if (!captured)
        {
            return;
        }

        auto* target = FindRegistration(captured);

        if (!target)
        {
            return;
        }

        // The captured target receives the release even
        // when the cursor is outside its bounds.
        const bool releasedInside = captured->ContainsPoint(worldPosition);

        auto callback = target->callbacks.onReleased;

        if (callback)
        {
            callback(releasedInside);
        }
    }
}

void InteractionRouter2D::CancelAllInteractions()
{
    PhysicalGeometry2D* captured = mCapturedGeometry;
    mCapturedGeometry = nullptr;

    if (!captured)
    {
        return;
    }

    if (auto* target = FindRegistration(captured))
    {
        auto callback = target->callbacks.onCancelled;

        if (callback)
        {
            callback();
        }
    }
}

void InteractionRouter2D::Clear()
{
    CancelAllInteractions();
    mTargets.clear();
}

void InteractionRouter2D::ProcessPointerEvent(const PointerEvent2D& event)
{
}

void InteractionRouter2D::CancelPointerInteraction(std::uint64_t pointerId)
{
}

} // namespace rendering_engine
