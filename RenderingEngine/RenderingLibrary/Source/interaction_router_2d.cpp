
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

    for (auto it = mCapturedGeometries.begin();
        it != mCapturedGeometries.end();
        ++it)
    {
        if (it->second == geometry)
        {
            const PointerKey pointer = it->first;

            CancelPointerInteraction(pointer);
            break;
        }
    }

    mTargets.erase(geometry);
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
    if (event.button != MouseButton::Left)
    {
        return;
    }

    if (event.action == InputAction::Repeat)
    {
        return;
    }

    const PointerAction action = event.action == InputAction::Press ? PointerAction::Press : PointerAction::Release;

    ProcessPointerEvent({
        PointerType::Mouse,
        0,
        action,
        glm::vec2{static_cast<float>(event.x), static_cast<float>(event.y)}
    });
}

void InteractionRouter2D::ProcessTouchEvent(const TouchEvent& event)
{
    PointerAction action;

    switch (event.phase)
    {
        case TouchPhase::Started:
            action = PointerAction::Press;
            break;

        case TouchPhase::Moved:
            action = PointerAction::Move;
            break;

        case TouchPhase::Ended:
            action = PointerAction::Release;
            break;

        case TouchPhase::Cancelled:
            action = PointerAction::Cancel;
            break;

        default:
            return;
    }

    ProcessPointerEvent({
        PointerType::Touch,
        event.id,
        action,
        event.position
        });
}

void InteractionRouter2D::CancelAllInteractions()
{
    while (!mCapturedGeometries.empty())
    {
        const PointerKey pointer = mCapturedGeometries.begin()->first;

        CancelPointerInteraction(pointer);
    }
}

void InteractionRouter2D::Clear()
{
    CancelAllInteractions();
    mTargets.clear();
}

void InteractionRouter2D::ProcessPointerEvent(const PointerEvent2D& event)
{
    const PointerKey pointer{
        event.type,
        event.pointerId
    };

    if (event.action == PointerAction::Cancel)
    {
        CancelPointerInteraction(pointer);
        return;
    }

    const auto camera = mScene.GetActiveCamera2D();

    if (!camera)
    {
        CancelAllInteractions();
        return;
    }

    const glm::vec2 worldPosition = camera->ScreenToWorld(event.screenPosition);

    switch (event.action)
    {
        case PointerAction::Press:
        {
            // This pointer already holds a capture.
            if (mCapturedGeometries.find(pointer) !=
                mCapturedGeometries.end())
            {
                return;
            }

            auto* geometry = FindTarget(worldPosition);

            if (!geometry)
            {
                return;
            }

            // A target can be captured by only one pointer.
            for (const auto& capture : mCapturedGeometries)
            {
                if (capture.second == geometry)
                {
                    return;
                }
            }

            auto* target = FindRegistration(geometry);

            if (!target)
            {
                return;
            }

            mCapturedGeometries.emplace(pointer, geometry);

            auto callback = target->callbacks.onPressed;

            if (callback)
            {
                callback();
            }

            break;
        }

        case PointerAction::Move:
            // Preserve capture while the pointer is moving.
            // Movement callbacks may be introduced later.
            break;

        case PointerAction::Release:
        {
            const auto found = mCapturedGeometries.find(pointer);

            if (found == mCapturedGeometries.end())
            {
                return;
            }

            auto* geometry = found->second;

            // Clear capture before invoking user code.
            mCapturedGeometries.erase(found);

            auto* target = FindRegistration(geometry);

            if (!target)
            {
                return;
            }

            const bool releasedInside =
                geometry->ContainsPoint(worldPosition);

            auto callback = target->callbacks.onReleased;

            if (callback)
            {
                callback(releasedInside);
            }

            break;
        }

        case PointerAction::Cancel:
            break;
        }
}

void InteractionRouter2D::CancelPointerInteraction(const PointerKey& pointer)
{
    const auto found = mCapturedGeometries.find(pointer);

    if (found == mCapturedGeometries.end())
    {
        return;
    }

    auto* geometry = found->second;

    mCapturedGeometries.erase(found);

    if (auto* target = FindRegistration(geometry))
    {
        auto callback = target->callbacks.onCancelled;

        if (callback)
        {
            callback();
        }
    }
}

} // namespace rendering_engine
