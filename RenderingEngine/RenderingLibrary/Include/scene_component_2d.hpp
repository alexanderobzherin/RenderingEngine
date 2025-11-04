// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rendering_engine
{
/**
 * @class SceneComponent2D
 * @brief Represents a 2D transformable scene component with position, rotation, and scale.
 *
 * SceneComponent2D holds position (in 2D), rotation (in degrees, counterclockwise), and scale.
 * It provides methods to set and get each component and maintains the current model matrix.
 * 
 * The engine uses a left-handed coordinate system:
 * - X+ : Right
 * - Y+ : Up
 *
 * The model matrix is updated automatically after any property change, and can be used to transform object-local
 * coordinates into world (scene) coordinates.
 *
 * Transform order is: scale, then rotate, then translate (S→R→T), matching typical 2D graphics pipelines.
 */
class SceneComponent2D
{
public:
    /**
     * @brief Constructs a SceneComponent2D at the origin, with zero rotation and unit scale.
     */
    SceneComponent2D();

    /**
     * @brief Destructor.
     */
    virtual ~SceneComponent2D() = default;

    /**
     * @brief Sets the position in 2D space.
     * @param position New position as glm::vec2.
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
     * @brief Gets the model matrix for this component.
     * @return The model matrix as glm::mat4.
     *
     * @note Use this to transform object-local coordinates into world (scene) coordinates.
     */
    const glm::mat4& GetModelMatrix() const { return mModelMatrix; }

    /**
     * @brief Updates the model matrix from the current position, rotation, and scale.
     * @details Called automatically after property changes, but can be called manually if needed.
     * @see SetPosition(), SetRotation(), SetScale()
     */
    void UpdateModelMatrix();

protected:
    glm::vec2 mPosition;
    float mRotation;
    glm::vec2 mScale;

    glm::mat4 mModelMatrix;
};

} // namespace rendering_engine