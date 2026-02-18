// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"

namespace rendering_engine
{
class Rectangle2D;
class TextBlock2D;

/**
 * @class StatsOverlay
 * @brief Displays real-time frame statistics as a 2D overlay.
 *
 * StatsOverlay renders a simple performance panel containing:
 * - FPS
 * - Frame duration (ms)
 * - Update time (ms)
 * - Draw time (ms)
 *
 * The overlay is drawn in screen space using Rectangle2D and TextBlock2D
 * subobjects. Values are updated every frame using FrameMetrics obtained
 * from the active application.
 *
 * Intended for debugging and performance diagnostics.
 */
class RE_API StatsOverlay : public Actor2D
{
public:
	/**
	 * @brief Constructs a StatsOverlay actor.
	 *
	 * Reads configuration options and prepares internal state.
	 *
	 * @param scene Reference to the owning scene.
	 */
	StatsOverlay(Scene& scene);
	/**
	 * @brief Creates and positions overlay visual elements.
	 *
	 * Spawns background rectangle and label/value text blocks,
	 * attaches them to the root component, and arranges them
	 * in a two-column layout.
	 */
	void Initialize() override;
	/**
	 * @brief Updates displayed statistics each frame.
	 *
	 * Retrieves FrameMetrics from the active application and
	 * updates value text blocks accordingly.
	 *
	 * @param deltaTime Time elapsed since previous frame (ms).
	 */
	void Update(float deltaTime) override;

private:
	Rectangle2D* mBackGround;

	TextBlock2D* mFPSLabel;
	TextBlock2D* mFPSValue;
	
	TextBlock2D* mFrameLabel;
	TextBlock2D* mFrameValue;

	TextBlock2D* mUpdateLabel;
	TextBlock2D* mUpdateValue;

	TextBlock2D* mDrawLabel;
	TextBlock2D* mDrawValue;

	glm::vec4 mBgColor{ 0.0f, 0.0f, 1.0f, 0.9f };
	glm::vec4 mLabelColor{ 0.5f, 0.5f, 0.5f, 1.0f };
	glm::vec4 mValueColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	bool bUseSmoothedFPS = true;

	float mTimeAccumulator = 0.0f;
	const float mUpdateInterval = 0.25f; // seconds
};

} // namespace rendering_engine
