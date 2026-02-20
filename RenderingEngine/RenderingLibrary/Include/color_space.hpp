// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "rendering_engine_export.hpp"

#include <glm/glm.hpp>

namespace rendering_engine
{
/**
 * @brief Color space conversion utilities (sRGB <-> linear).
 *
 * The rendering engine operates internally in linear color space.
 * Use these helpers when converting colors from UI/design tools
 * (typically sRGB) to linear space before rendering.
 */
class RE_API ColorSpace
{
public:
	/**
	 * @brief Converts a single sRGB channel to linear space.
	 */
	static float SRGBToLinear(float c);
	/**
	 * @brief Converts an sRGB RGB color to linear space.
	 */
	static glm::vec3 SRGBToLinear(const glm::vec3& color);
	/**
	 * @brief Converts a single linear channel to sRGB space.
	 */
	static float LinearToSRGB(float c);
	/**
	 * @brief Converts a single linear channel to sRGB space.
	 */
	static glm::vec3 LinearToSRGB(const glm::vec3& color);
};

} // namespace rendering_engine
