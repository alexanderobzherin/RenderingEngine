// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace rendering_engine
{
class IApplication;
class SceneComponent2D;

/**
 * @class Camera2D
 * @brief Represents a 2D camera with position, rotation, and zoom control.
 *
 * Camera2D defines the view in a 2D scene, using a SceneComponent2D for transform (position, rotation, zoom).
 * The camera provides basic pan, rotation, and uniform zoom features.
 *
 * The engine uses a left-handed coordinate system:
 * - X+ : Right
 * - Y+ : Up
 * Rotation is counterclockwise, in degrees, around the Z axis (out of the screen).
 *
 * By default, the camera is at the origin, with zero rotation and unit zoom.
 *
 * @see rendering_engine::SceneComponent2D
 */
class Camera2D
{
public:
	/**
	 * @brief Constructs a 2D camera at the origin, with no rotation and unit zoom.
	 */
	Camera2D(IApplication& app);

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~Camera2D() = default;

	/**
	 * @brief Initializes the camera (calls Reset()).
	 */
	virtual void Initialize();

	/**
	 * @brief Updates the camera logic (no-op for now).
	 * @param deltaTime Time since last update (seconds).
	 */
	virtual void Update(float deltaTime);

	/**
	 * @brief Sets the viewport dimensions used to compute the projection matrix.
	 *
	 * This defines the visible area of the camera in screen-space units.
	 * The projection matrix will be recomputed based on these dimensions.
	 *
	 * @param width  Viewport width in pixels or world units.
	 * @param height Viewport height in pixels or world units.
	 */
	void SetViewportSize(float width, float height);

	/**
	 * @brief Returns the current orthographic projection matrix.
	 *
	 * The matrix is built from the camera’s viewport size, and is typically
	 * multiplied with the view (world) matrix to transform coordinates into clip space.
	 *
	 * @return 4×4 orthographic projection matrix.
	 */
	glm::mat4 GetProjectionMatrix() const;

	/**
	 * @brief Sets the camera position.
	 * @param position 2D position (glm::vec2).
	 */
	void SetPosition(const glm::vec2& position);

	/**
	 * @brief Sets the camera rotation angle in degrees (counterclockwise, around Z).
	 * @param angleDegrees Angle in degrees.
	 */
	void SetRotation(float angleDegrees);

	/**
	 * @brief Sets the camera zoom (uniform scale).
	 * @param zoom Zoom factor (1.0 = no zoom).
	 */
	void SetZoom(float zoom);

	/**
	 * @brief Resets the camera to the default view (origin, zero rotation, unit zoom).
	 */
	void Reset();

	/**
	 * @brief Gets the camera position.
	 * @return 2D position (glm::vec2).
	 */
	glm::vec2 GetPosition() const;

	/**
	 * @brief Gets the camera rotation angle in degrees.
	 * @return Rotation angle (degrees).
	 */
	float GetRotation() const;

	/**
	 * @brief Gets the current zoom factor.
	 * @return Zoom factor.
	 */
	float GetZoom() const;

	/**
	 * @brief Gets the world-view (model) matrix for the camera.
	 * @return Model/view matrix (glm::mat3).
	 * @note Use this to transform between world and screen space for rendering.
	 */
	const glm::mat4& GetWorldView() const;

protected:
	glm::mat4 ComputeOrthographicMatrix(
		float left, float right,
		float top, float bottom,
		float nearPlane, float farPlane) const;

protected:
	IApplication& mApp;
	std::unique_ptr<SceneComponent2D> mSceneComponent;
	glm::vec2 mViewportSize;
};

} // namespace rendering_engine
