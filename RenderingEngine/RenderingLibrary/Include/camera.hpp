// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "rendering_engine_export.hpp"
#include "vector_definitions.hpp"

namespace rendering_engine
{
class IApplication;
class SceneComponent;

/**
 * @class Camera
 * @brief Represents a 3D perspective camera with world transform and projection settings.
 *
 * The Camera defines the viewpoint in the 3D scene, including position and orientation (via a SceneComponent)
 * and projection parameters (field of view, aspect ratio, near/far planes). The camera supports standard
 * view matrix construction using GLM and can be subclassed for additional camera types (third-person, orbit, etc).
 *
 * @section CoordinateSystem Coordinate System
 * The rendering engine uses a left-handed coordinate system:
 * - X+ : Forward (world forward direction)
 * - Y+ : Right
 * - Z+ : Up
 * See @ref rendering_engine::axes for axis definitions.
 *
 * By default, the camera looks along the world Y+ direction after Reset().
 *
 * @note
 * - Angles (Euler) are in degrees, matching typical editor/engine conventions.
 * - The camera transform is provided by a SceneComponent.
 * - The camera generates a perspective projection matrix using a vertical field of view.
 *
 * @see rendering_engine::SceneComponent
 * @see rendering_engine::axes
 */
class RE_API Camera
{
public:
	/**
	 * @brief Constructs a camera for the given application context.
	 * @param app Reference to the IApplication (for screen settings, etc.).
	 */
	Camera(IApplication& app);

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~Camera() = default;

	/**
	 * @brief Initializes camera parameters and matrices.
	 */
	virtual void Initialize();

	/**
	 * @brief Updates camera logic, recalculates view/projection matrices as needed.
	 * @param deltaTime Elapsed time since last update (seconds).
	 */
	virtual void Update(float deltaTime);

	/**
	 * @brief Sets the camera position in world space.
	 * @param position New position (glm::vec3).
	 */
	void SetPosition(const glm::vec3& position);

	/**
	 * @brief Sets the camera rotation via quaternion.
	 * @param rotation New rotation (glm::quat).
	 */
	void SetRotation(const glm::quat& rotation);

	/**
	 * @brief Sets the camera rotation via Euler angles (degrees).
	 * @param rotation Euler angles (pitch, yaw, roll), in degrees.
	 */
	void SetRotation(const glm::vec3& rotation);

	/**
	 * @brief Gets the camera position.
	 */
	glm::vec3 GetPosition() const;

	/**
	 * @brief Gets the camera rotation as a quaternion.
	 */
	glm::quat GetRotationQuat() const;

	/**
	 * @brief Gets the camera rotation as Euler angles (degrees).
	 */
	glm::vec3 GetRotation() const;

	/**
	 * @brief Gets the camera's forward direction vector.
	 */
	glm::vec3 GetForward() const;

	/**
	 * @brief Gets the camera's up direction vector.
	 */
	glm::vec3 GetUp() const;

	/**
	 * @brief Gets the camera's right direction vector.
	 */
	glm::vec3 GetRight() const;

	/**
	 * @brief Gets the camera view matrix.
	 */
	const glm::mat4& ViewMatrix() const;

	/**
	 * @brief Gets the camera projection matrix.
	 */
	const glm::mat4& ProjectionMatrix() const;

	/**
	 * @brief Gets the combined view-projection matrix.
	 */
	glm::mat4 ViewProjectionMatrix() const;

	/**
	 * @brief Resets the camera to the default position and rotation.
	 */
	void Reset();

	/**
	 * @brief Updates the camera's projection matrix (based on FOV, aspect, near/far).
	 */
	void UpdateProjectionMatrix();

	/**
	 * @brief Updates the camera's view matrix (based on position/orientation).
	 */
	void UpdateViewMatrix();

	Camera(const Camera& rhs) = delete;
	Camera& operator=(const Camera& rhs) = delete;

protected:
	IApplication& mApp;

	std::unique_ptr<SceneComponent> mSceneComponent;

	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

	static const float sDefaultFieldOfView;
	static const float sDefaultAspectRatio;
	static const float sDefaultNearPlaneDistance;
	static const float sDefaultFarPlaneDistance;

	float mFieldOfView;
	float mAspectRatio;
	float mNearPlaneDistance;
	float mFarPlaneDistance;
};
} //rendering_engine