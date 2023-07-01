#pragma once

#include <cstring>
#include <memory>
#include <iostream>
#include <cstdint>
#include <fstream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "vertex_declarations.hpp"
#include "image_data.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace rendering_engine
{
namespace vulkan_renderer
{
class VulkanRenderer;

class VulkanImageData
{
public:
	VulkanImageData(VulkanRenderer* renderer, std::shared_ptr<ImageData> colorTextureImageData);
	virtual ~VulkanImageData();

	void Initialize();
	void Shutdown();

	std::shared_ptr<rendering_engine::ImageData> GetImageData();
	VkImage GetVulkanTextureImage();
	VkImageView GetVulkanTextureImageView();
	VkDeviceMemory GetVulkanTextureImageMemory();
	std::uint32_t GetMipmapLevels();
	VkSampler GetTextureSampler();

private:
	VulkanImageData(VulkanImageData const& src);
	VulkanImageData& operator=(VulkanImageData const& rhs);

protected:
	void CreateTextureImage();
	void CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateTextureImageView();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels);
	void CreateTextureSampler();

protected:
	VulkanRenderer* mRenderer;
	std::shared_ptr<ImageData> mImageData;
	std::uint32_t mMipmapLevels;
	VkImage mTextureImage;
	VkDeviceMemory mTextureImageMemory;
	VkImageView mTextureImageView;
	VkSampler mTextureSampler;
};

} // vulkan_renderer
} //rendering_engine