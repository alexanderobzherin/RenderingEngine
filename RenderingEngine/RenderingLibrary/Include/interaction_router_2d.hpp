
#pragma once

#include "rendering_engine_export.hpp"
#include "input_event.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <map>
#include <utility>

namespace rendering_engine
{

class Scene;
class PhysicalGeometry2D;

/**
 * @brief Routes pointer interactions to registered 2D geometry.
 *
 * Performs hit-testing and selects the target with the highest
 * interaction priority. For equal priorities, the most recently
 * registered target takes precedence.
 *
 * Maintains pointer capture until release or cancellation.
 * Registered geometry is referenced but not owned.
 */
class RE_API InteractionRouter2D
{
using PointerKey = std::pair<PointerType, std::uint64_t>;

public:
    /**
     * @brief Callbacks invoked during a pointer interaction.
     */
    struct Callbacks
    {
        /// Invoked when a pointer presses the target.
        std::function<void()> onPressed;

        /// Invoked when the captured pointer is released.
        /// The argument indicates whether release occurred inside the target.
        std::function<void(bool)> onReleased;

        /// Invoked when an active interaction is cancelled.
        std::function<void()> onCancelled;
    };

    /**
     * @brief Constructs an interaction router associated with a scene.
     * @param scene Scene containing the interactive geometry.
     */
    explicit InteractionRouter2D(Scene& scene);

    /**
     * @brief Registers geometry as an interactive target.
     *
     * The geometry must already be registered in the scene's
     * collision world.
     *
     * @param geometry Non-owning pointer to the geometry.
     * @param callbacks Interaction callbacks.
     * @param priority Selection priority; higher values take precedence.
     *                 Equal priorities favor the latest registration.
     *
     * @throws std::invalid_argument If geometry is null, not registered
     *         in the collision world, or already registered as a target.
     */
    void RegisterTarget(
        PhysicalGeometry2D* geometry,
        Callbacks callbacks,
        int priority = 0);

    /**
     * @brief Removes a geometry from interaction routing.
     *
     * Cancels an active interaction if the geometry currently
     * holds pointer capture.
     *
     * @param geometry Geometry to unregister.
     */
    void UnregisterTarget(PhysicalGeometry2D* geometry);

    /**
     * @brief Processes a mouse button event for 2D interactions.
     *
     * Currently handles left-button press and release events.
     * Other mouse buttons and repeat events are ignored.
     *
     * Uses the active 2D camera to convert screen coordinates
     * into world coordinates for hit-testing.
     *
     * @param event Mouse button event to process.
     */
    void ProcessMouseButtonEvent(const MouseButtonEvent& event);

    /**
     * @brief Processes a touchscreen contact event
     *        for 2D interactions.
     *
     * Translates the touch event into a device-independent
     * pointer event and routes it to interactive geometry.
     *
     * @param event Touch event to process.
     */
    void ProcessTouchEvent(const TouchEvent& event);

    /**
     * @brief Cancels all active pointer interactions.
     * @note Invokes onCancelled callbacks for captured targets.
     */
    void CancelAllInteractions();

    /**
     * @brief Removes all registered targets and clears pointer capture.
     * @note Active interactions are cancelled before clearing.
     */
    void Clear();
private:
    // Will be public API once fully implemented
    /**
     * @brief Processes a device-independent pointer event.
     * @param event Pointer action and screen-space position.
     */
    void ProcessPointerEvent(const PointerEvent2D& event);
    /**
     * @brief Cancels the active interaction for a specific pointer.
     * @param pointerId Identifier of the pointer to cancel.
     * @note Invokes the target's onCancelled callback if the pointer
     *       currently holds a captured target.
     */
    void CancelPointerInteraction(const PointerKey& pointer);

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

    // Non-owning captured geometry for each active pointer.
    std::map<PointerKey, PhysicalGeometry2D*> mCapturedGeometries;

    std::uint64_t mNextOrder = 0;
};

} // namespace rendering_engine
