#pragma once

#include <vector>
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace rendering_engine
{
struct Vertex 
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
};

}