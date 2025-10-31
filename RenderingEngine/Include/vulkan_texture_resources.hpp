// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_texture_render_resources.hpp"
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
class VulkanRenderer;
class ImageData;

/**
 * @class VulkanTextureResources
 * @brief Vulkan-specific implementation of ITextureRenderResources.
 *
 * This class manages texture memory and resources on the GPU using Vulkan.
 */
class VulkanTextureResources : public ITextureRenderResources
{
public:
	/**
	 * @brief Vulkan-specific implementation of ITextureRenderResources.
	 *
	 * @internal
	 * Handles uploading, managing, and releasing texture data on the GPU using Vulkan.
	 * This class is used internally by the rendering engine and is not intended for public use.
	 *
	 * @note Serves as a backend implementation; other graphics APIs can provide their own versions.
	 */
	VulkanTextureResources(VulkanRenderer* renderer);

	/**
	 * @brief Uploads the texture data from CPU to GPU memory using Vulkan.
	 *
	 * @param data The image data to upload.
	 */
	void LoadToGPU(const ImageData& data) override;

	/**
	 * @brief Releases the texture data from GPU memory using Vulkan.
	 */
	void ReleaseFromGPU() override;

	/**
	 * @brief Checks if the texture is currently loaded in GPU memory.
	 *
	 * @return True if the texture is in GPU memory, false otherwise.
	 */
	bool IsTextureLoadedInGPU() override;

	/**
	 * @brief Returns the size of the texture in GPU memory in bytes.
	 *
	 * @return Size in bytes.
	 */
	size_t GetSizeInGPUBytes() const override;

	/**
	 * @brief Returns the Vulkan image view associated with this texture resource.
	 *
	 * This image view is typically used for binding the texture to shaders via descriptor sets.
	 *
	 * @return The VkImageView handle for this texture resource.
	 */
	VkImageView GetVkImageView() const { return mTextureImageView; }

	/**
	 * @brief Returns the Vulkan sampler associated with this texture resource.
	 *
	 * This sampler is used to define how the texture is sampled in shaders (e.g., filtering, addressing modes).
	 *
	 * @return The VkSampler handle for this texture resource.
	 */
	VkSampler   GetVkSampler()   const { return mTextureSampler; }

private:
	VulkanTextureResources(VulkanTextureResources const& src);
	VulkanTextureResources& operator=(VulkanTextureResources const& rhs);

protected:
	void CreateTextureImage();
	void CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void CreateTextureImageView();
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels);
	void CreateTextureSampler();

protected:
	bool bIsTextureLoadedInGPU;
	VulkanRenderer* mRenderer;
	const ImageData* mImageData;
	std::uint32_t mMipmapLevels;
	VkImage mTextureImage;
	VkDeviceMemory mTextureImageMemory;
	VkImageView mTextureImageView;
	VkSampler mTextureSampler;
	VkDeviceSize mGpuMemorySize;
};

}