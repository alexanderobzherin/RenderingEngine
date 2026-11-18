
#pragma once

#include "rendering_engine_export.hpp"
#include "input_event.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <functional>
#include <unordered_map>

namespace rendering_engine
{

class Scene;
class PhysicalGeometry2D;

/**
 * @brief Routes pointer interactions to registered 2D geometry.
 *
 * Performs hit-testing, selects the highest-priority target,
 * and maintains pointer capture until release or cancellation.
 *
 * Registered geometry is referenced but not owned.
 */
class RE_API InteractionRouter2D
{
public:
    /**
     * @brief Callbacks invoked during a pointer interaction.
     */
    struct Callbacks
    {
        /// Invoked when a pointer presses the target.
        std::function<void()> onPressed;

        /// Invoked on release; true if released inside the target.
        std::function<void(bool)> onReleased;

        /// Invoked when an active interaction is cancelled.
        std::function<void()> onCancelled;
    };

    /**
     * @brief Constructs a router associated with a scene.
     */
    explicit InteractionRouter2D(Scene& scene);

    /**
     * @brief Registers geometry as an interactive target.
     * @param geometry Non-owning pointer to registered scene geometry.
     * @param callbacks Interaction callbacks.
     * @param priority Selection priority; higher values take precedence.
     * @throws std::invalid_argument If geometry is null, unregistered,
     *         or already registered.
     */
    void RegisterTarget(
        PhysicalGeometry2D* geometry,
        Callbacks callbacks,
        int priority = 0);

    /**
     * @brief Removes a target from interaction routing.
     * @note Cancels an active interaction involving this target.
     */
    void UnregisterTarget(PhysicalGeometry2D* geometry);

    /**
     * @brief Processes a device-independent pointer event.
     * @param event Pointer action and screen-space position.
     */
    void ProcessPointerEvent(const PointerEvent2D& event);

    /**
     * @brief Processes a mouse button event.
     *
     * Converts mouse input into the common pointer-event format.
     */
    void ProcessMouseButtonEvent(const MouseButtonEvent& event);

    /**
     * @brief Cancels the active interaction for a specific pointer.
     * @param pointerId Identifier of the pointer to cancel.
     * @note Invokes the target's onCancelled callback if the pointer
     *       currently holds a captured target.
     */
    void CancelPointerInteraction(std::uint64_t pointerId);

    /**
     * @brief Cancels all active pointer interactions.
     * @note Invokes onCancelled callbacks for captured targets.
     */
    void CancelActiveInteractions();

    /**
     * @brief Removes all registered targets and clears pointer capture.
     * @note Active interactions are cancelled before clearing.
     */
    void Clear();

private:
    struct Target
    {
        PhysicalGeometry2D* geometry = nullptr;
        Callbacks callbacks;
        int priority = 0;
        std::uint64_t order = 0;
    };

    PhysicalGeometry2D* FindTarget(const glm::vec2& worldPosition) const;

    Target* FindRegistration(PhysicalGeometry2D* geometry);

    const Target* FindRegistration(PhysicalGeometry2D* geometry) const;

private:
    Scene& mScene;

    std::unordered_map<PhysicalGeometry2D*, Target> mTargets;

    // Non-owning pointer to the currently captured geometry.
    PhysicalGeometry2D* mCapturedGeometry = nullptr;

    std::uint64_t mNextOrder = 0;
};

} // namespace rendering_engine
