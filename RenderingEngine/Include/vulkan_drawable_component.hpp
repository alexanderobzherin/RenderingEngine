#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <optional>
#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "drawable_component.hpp"
#include "vulkan_vertex_declarations.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace rendering_engine
{
namespace vulkan_renderer
{
class VulkanRenderer;
class VulkanImageData;

class VulkanDrawableComponent : public DrawableComponent
{
public:
	VulkanDrawableComponent(VulkanRenderer* renderer);
	virtual ~VulkanDrawableComponent();

	virtual void Initialize();
	virtual void Shutdown();

	virtual void Update(float delta);
	virtual void Draw();

private:
	VulkanDrawableComponent(const VulkanDrawableComponent& rhs);
	VulkanDrawableComponent& operator=(const VulkanDrawableComponent& rhs);

protected:
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void CreateDescriptorSet(VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout);
	void UpdateUniformBuffer();

protected:
	VulkanRenderer* mRenderer;
	std::shared_ptr<VulkanImageData> mVulkanColorTexture;

	VkBuffer mVertexBuffer;
	VkDeviceMemory mVertexBufferMemory;
	VkBuffer mIndexBuffer;
	VkDeviceMemory mIndexBufferMemory;
	std::vector<VkBuffer> mUniformBuffers;
	std::vector<VkDeviceMemory> mUniformBuffersMemory;
	std::vector<VkDescriptorSet> mDescriptorSets;
	UniformBufferObject mUbo;

};

} // vulkan_renderer
} //rendering_engine