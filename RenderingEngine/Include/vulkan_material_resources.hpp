// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_material_render_resources.hpp" 

#include <utility>
#include <vulkan/vulkan.h>

namespace rendering_engine
{
class Material;
class VulkanRenderer;

/**
 * @brief Vulkan-specific implementation of material render resources.
 *
 * This class manages Vulkan GPU resources required to render a specific Material.
 * It provides access to the Vulkan descriptor set layout, pipeline layout, and graphics pipeline
 * associated with a given material. The resource creation is delegated to the owning
 * `VulkanRenderer` instance.
 *
 * The `Material` class calls `Initialize` and `Shutdown` to manage the lifecycle of these resources.
 */
class VulkanMaterialResources : public IMaterialRenderResources
{
public:
	/**
	 * @brief Constructs a VulkanMaterialResources instance.
	 *
	 * @param renderer Pointer to the VulkanRenderer that provides creation utilities.
	 */
	VulkanMaterialResources(VulkanRenderer* renderer);

	/**
	 * @brief Initializes Vulkan-specific GPU resources for the material.
	 *
	 * Loads compiled SPIR-V shader binaries and creates descriptor set layout and pipeline.
	 *
	 * @param material Pointer to the owning Material instance.
	 */
	void Initialize(Material* material) override;

	/**
	 * @brief Releases all Vulkan GPU resources associated with this material.
	 *
	 * This should be called when the renderer shuts down or resources are rebuilt.
	 */
	void Shutdown() override;

	/**
	 * @brief Gets the Vulkan descriptor set layout for the material.
	 *
	 * @return Vulkan handle to the descriptor set layout.
	 */
	VkDescriptorSetLayout GetDescriptorSetLayout() const;

	/**
	 * @brief Gets the Vulkan pipeline layout used by the material.
	 *
	 * @return Vulkan handle to the pipeline layout.
	 */
	VkPipelineLayout GetPipelineLayout() const;

	/**
	 * @brief Gets the Vulkan graphics pipeline used by the material.
	 *
	 * @return Vulkan handle to the pipeline.
	 */
	VkPipeline GetPipeline() const;

private:
	VulkanRenderer* mRenderer;
	VkDescriptorSetLayout mDescriptorSetLayout;
	std::pair<VkPipelineLayout, VkPipeline> mPipelinePair;
};

} //namespace rendering_engine