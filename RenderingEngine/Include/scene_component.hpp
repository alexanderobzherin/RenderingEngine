// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <memory>
#include "vector_definitions.hpp"

namespace rendering_engine
{

/**
 * @class SceneComponent
 * @brief Represents a 3D transformable scene component with position, rotation, and scale.
 *
 * SceneComponent holds the core kinematic properties required for positioning and orienting objects in a 3D scene.
 * It provides methods to set and get position, rotation (as both quaternion and Euler angles, in radians), and scale.
 * The world transformation matrix is updated automatically when any of these properties change.
 * Axis direction vectors (forward, right, up) are derived from the current rotation.
 *
 * @note
 * - Euler angles are expected in radians, using the convention (pitch, yaw, roll).
 * - The rendering engine uses a left-handed coordinate system:
 * - X+ : Forward (world forward direction)
 * - Y+ : Right
 * - Z+ : Up
 * - Copy and assignment are disabled for this class.
 */
class SceneComponent
{
public:
	/**
	 * @brief Constructs a SceneComponent with default transform (origin, identity rotation, unit scale).
	 */
	SceneComponent();

	/**
	 * @brief Destructor.
	 */
	virtual ~SceneComponent() = default;

	/**
	 * @brief Updates the world transformation matrix from the current position, rotation, and scale.
	 * @details Normally called automatically after setters; call manually if you modify transform members directly.
	 */
	void UpdateWorldMatrix();

	/**
	 * @brief Returns the world transformation matrix (model matrix).
	 * @return The current world/model matrix (glm::mat4).
	 */
	const glm::mat4& GetWorldMatrix(); // const;

	/**
	 * @brief Sets the position of the component in world space.
	 * @param position The new position as a glm::vec3.
	 */
	void SetPosition(const glm::vec3& position);

	/**
	 * @brief Sets the rotation using a quaternion.
	 * @param rotation The rotation as a glm::quat.
	 * @note Automatically updates the internal Euler angles (in radians).
	 */
	void SetRotation(const glm::quat& rotation);

	/**
	 * @brief Sets the rotation using Euler angles (in degrees).
	 * @param rotation Euler angles in degrees: (pitch, yaw, roll).
	 *        - pitch: rotation around the Right (Y) axis
	 *        - yaw:   rotation around the Up (Z) axis
	 *        - roll:  rotation around the Forward (X) axis
	 * @note Angles are converted internally to radians.
	 */
	void SetRotation(const glm::vec3& rotation);

	/**
	 * @brief Sets the scale for each dimension.
	 * @param scale The new scale as a glm::vec3.
	 */
	void SetScale(const glm::vec3& scale);

	/**
	 * @brief Gets the current position.
	 * @return The position as a glm::vec3.
	 */
	const glm::vec3& GetPosition() const { return mPosition; }

	/**
	 * @brief Gets the current rotation as a quaternion.
	 * @return The rotation as a glm::quat.
	 */
	const glm::quat& GetRotationQuat() const { return mRotation; }

	/**
	 * @brief Gets the current rotation as Euler angles (in radians).
	 * @return Euler angles (pitch, yaw, roll), where:
	 *   - pitch: rotation around the Right (Y) axis
	 *   - yaw:   rotation around the Up (Z) axis
	 *   - roll:  rotation around the Forward (X) axis
	 */
	const glm::vec3& GetRotation() const { return mEulerRotation; }

	/**
	 * @brief Gets the current scale.
	 * @return The scale as a glm::vec3.
	 */
	const glm::vec3& GetScale() const { return mScale; }

	/**
	 * @brief Returns the forward (local X+) direction vector in world space.
	 * @return The forward direction as a glm::vec3.
	 * @see rendering_engine::axes::ForwardVector
	 */
	glm::vec3 GetForward() const;

	/**
	 * @brief Returns the right (local Y+) direction vector in world space.
	 * @return The right direction as a glm::vec3.
	 * @see rendering_engine::axes::RightVector
	 */
	glm::vec3 GetRight() const;

	/**
	 * @brief Returns the up (local Z+) direction vector in world space.
	 * @return The up direction as a glm::vec3.
	 * @see rendering_engine::axes::UpVector
	 */
	glm::vec3 GetUp() const;

	SceneComponent(const SceneComponent& rhs) = delete;
	SceneComponent& operator=(const SceneComponent& rhs) = delete;

protected:
	glm::vec3 mPosition;
	glm::quat mRotation;
	glm::vec3 mEulerRotation;
	glm::vec3 mScale;

	glm::mat4 mWorldMatrix;
};

} //rendering_engine