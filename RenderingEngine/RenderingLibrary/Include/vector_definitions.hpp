// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace rendering_engine
{

/**
 * @namespace rendering_engine::axes
 * @brief Commonly used 3D axis and direction vectors.
 *
 * Coordinate system:
 * - X+ (Forward)
 * - Y+ (Right)
 * - Z+ (Up)
 *
 * Vectors:
 * - ForwardVector: (1, 0, 0)
 * - RightVector:   (0, 1, 0)
 * - UpVector:      (0, 0, 1)
 * etc.
 */
namespace axes
{
static const glm::vec3 ZeroVector = glm::vec3(0.0f, 0.0f, 0.0f);
static const glm::vec3 OneVector = glm::vec3(1.0f, 1.0f, 1.0f);

static const glm::vec3 UpVector = glm::vec3(0.0f, 0.0f, 1.0f);
static const glm::vec3 DownVector = glm::vec3(0.0f, 0.0f, -1.0f);

static const glm::vec3 ForwardVector = glm::vec3(1.0f, 0.0f, 0.0f);
static const glm::vec3 BackwardVector = glm::vec3(-1.0f, 0.0f, 0.0f);

static const glm::vec3 RightVector = glm::vec3(0.0f, 1.0f, 0.0f);
static const glm::vec3 LeftVector = glm::vec3(0.0f, -1.0f, 0.0f);
} // namespace axes

} // namespace rendering_engine
