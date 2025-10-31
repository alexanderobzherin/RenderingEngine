#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <vector>
#include <array>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace rendering_engine
{
	class VulkanRenderer;
	class TestSprite2D
	{
	public:
		TestSprite2D();
		~TestSprite2D() = default;

		void Initialize(VulkanRenderer* renderer);
		void Update(float deltaTime);
		void Draw();

		void Shutdown();

	protected:
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateGraphicsPipeline();


	private:
		VulkanRenderer* mRenderer;

		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		struct Vertex {
			glm::vec2 pos;
			glm::vec3 color;
		};

		std::vector<Vertex> mVertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},  // bottom left
			{{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // bottom right
			{{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},  // top right
			{{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}   // top left
		};

		std::vector<uint16_t> mIndices = {
			0, 1, 2,  // first triangle
			2, 3, 0   // second triangle
		};

		VkBuffer mVertexBuffer;
		VkDeviceMemory mVertexBufferMemory;
		size_t mVertexBufferSize;

		VkBuffer mIndexBuffer;
		VkDeviceMemory mIndexBufferMemory;
		size_t mIndexBufferSize;
	};
}