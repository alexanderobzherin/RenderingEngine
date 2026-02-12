// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rendering_engine
{
/**
 * @class SceneComponent2D
 * @brief Represents a hierarchical 2D transform component.
 *
 * SceneComponent2D stores local 2D transformation state:
 *  - Position (x, y)
 *  - Rotation (degrees, counterclockwise around Z-axis)
 *  - Scale (x, y)
 *
 * It supports parent-child relationships, allowing hierarchical
 * transform propagation. When attached to a parent component,
 * its world transform is computed as:
 *
 *     World = ParentWorld * Local
 *
 * The engine uses a left-handed coordinate system:
 *  - X+ : Right
 *  - Y+ : Up
 *
 * Transform order is:
 *
 *     M = T * R * S
 *
 * which results in object-space vertices being scaled, then rotated,
 * then translated when applied.
 *
 * The component uses a dirty-flag mechanism. World matrices are
 * recomputed lazily when requested.
 *
 * @note
 *  - This component operates strictly in the 2D domain.
 *  - Rotation is always around the Z-axis.
 *  - Ownership and lifetime are managed externally (e.g., by Actor2D).
 *
 * @see Actor2D
 */
class RE_API SceneComponent2D
{
public:
    /**
     * @brief Constructs a SceneComponent2D with identity transform.
     *
     * Initial state:
     *  - Position = (0, 0)
     *  - Rotation = 0 degrees
     *  - Scale    = (1, 1)
     *  - No parent attached
     */
    SceneComponent2D();

    /**
     * @brief Destructor.
     */
    virtual ~SceneComponent2D() = default;

    /**
     * @brief Sets the local position of this component.
     *
     * Marks the component as dirty so that world transformation
     * will be recalculated when needed.
     *
     * @param position Local position in 2D space.
     */
    void SetPosition(const glm::vec2& position);

    /**
     * @brief Sets the rotation angle in degrees.
     * @param angleDegrees Angle in degrees (counterclockwise).
     */
    void SetRotation(float angleDegrees);

    /**
     * @brief Sets the scale in each dimension.
     * @param scale New scale as glm::vec2.
     */
    void SetScale(const glm::vec2& scale);

    /**
     * @brief Gets the current position.
     * @return The position as glm::vec2.
     */
    const glm::vec2& GetPosition() const { return mPosition; }

    /**
     * @brief Gets the current rotation angle (in degrees).
     * @return The rotation angle (degrees).
     */
    float GetRotation() const { return mRotation; }

    /**
     * @brief Gets the current scale.
     * @return The scale as glm::vec2.
     */
    const glm::vec2& GetScale() const { return mScale; }
    /**
     * @brief Returns the world-space position of this component.
     *
     * Extracted from the world transformation matrix.
     *
     * @return World position as glm::vec2.
     */
    glm::vec2 GetWorldPosition() const;
    /**
     * @brief Returns the accumulated world-space rotation.
     *
     * If attached to a parent, this includes parent rotation.
     *
     * @return World rotation angle in degrees.
     */
    float GetWorldRotation() const;
    /**
     * @brief Returns the accumulated world-space scale.
     *
     * If attached to a parent, this includes parent scaling.
     *
     * @return World scale as glm::vec2.
     */
    glm::vec2 GetWorldScale() const;
    /**
     * @brief Returns the world transformation matrix.
     *
     * If the component is marked dirty, the world matrix
     * will be recomputed before returning.
     *
     * @return 4x4 world transformation matrix.
     */
    const glm::mat4& GetWorldMatrix();

    /**
     * @brief Recomputes the local transformation matrix.
     *
     * The local matrix is constructed as:
     *
     *     Local = T * R * S
     *
     * This method does not compute the world matrix.
     * Called internally by UpdateWorldMatrix().
     */
    void UpdateLocalMatrix();
    /**
     * @brief Recomputes the world transformation matrix.
     *
     * Combines parent world matrix (if any) with the local matrix:
     *
     *     World = ParentWorld * Local
     *
     * Clears the dirty flag after update.
     */
    void UpdateWorldMatrix();

    /**
     * @brief Attaches this scene component to a parent scene component.
     *
     * After attachment, this component's transform becomes relative to the parent.
     * The world transformation is computed by combining the parent's world transform
     * with this component's local transform.
     *
     * Passing nullptr detaches the component and makes it a root-level component.
     *
     * @param parent Pointer to the parent SceneComponent2D, or nullptr to detach.
     */
    void AttachTo(SceneComponent2D* parent);

    SceneComponent2D(const SceneComponent2D& rhs) = delete;
    SceneComponent2D& operator=(const SceneComponent2D& rhs) = delete;

protected:
    glm::vec2 mPosition;
    float mRotation;
    glm::vec2 mScale;

    glm::mat4 mLocalMatrix;
    glm::mat4 mWorldMatrix;

    SceneComponent2D* mParent;
    bool bIsDirty;
};

} // namespace rendering_engine