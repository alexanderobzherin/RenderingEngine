// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "rendering_engine_export.hpp"

namespace rendering_engine
{
class IApplication;
class SceneComponent2D;

/**
 * @brief Represents a 2D camera with position, rotation, and zoom control.
 *
 * Defines the view of a 2D scene using a SceneComponent2D transform.
 * Supports screen-to-world coordinate conversion for 2D interactions.
 *
 * The engine's 2D coordinate convention:
 * - X increases to the right.
 * - Y increases downward on screen.
 * - Rotation is expressed in degrees around the Z axis.
 *
 * By default, the camera is at the origin, with zero rotation
 * and unit zoom.
 *
 * @see SceneComponent2D
 */
class RE_API Camera2D
{
public:
	/**
	 * @brief Constructs a 2D camera at the origin with unit zoom.
	 * @param app Application providing the initial viewport dimensions.
	 */
	Camera2D(IApplication& app);

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~Camera2D();

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
	 * @brief Sets the viewport dimensions.
	 *
	 * Used to calculate the orthographic projection matrix
	 * and convert screen coordinates into world coordinates.
	 *
	 * @param width Viewport width in pixels.
	 * @param height Viewport height in pixels.
	 */
	void SetViewportSize(float width, float height);

	/**
	 * @brief Returns the camera's orthographic projection matrix.
	 *
	 * The projection is calculated from the current viewport dimensions.
	 *
	 * @return 4x4 orthographic projection matrix.
	 */
	glm::mat4 GetProjectionMatrix() const;

	/**
	 * @brief Converts a viewport-local screen position into 2D world coordinates.
	 *
	 * Screen coordinates have their origin at the top-left corner
	 * of the viewport, with X increasing rightward and Y downward.
	 *
	 * The conversion accounts for the camera's projection and
	 * view transformations.
	 *
	 * @param screenPosition Position in viewport-local coordinates.
	 * @return Corresponding position in 2D world coordinates.
	 *
	 * @throws std::runtime_error If viewport dimensions are non-positive
	 *         or the camera zoom is zero.
	 */
	glm::vec2 ScreenToWorld(const glm::vec2& screenPosition) const;

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
	 * @brief Returns the camera's world-view transformation matrix.
	 *
	 * @return Reference to the 4x4 transformation matrix.
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
