#include "vulkan_material_resources.hpp"
#include "vulkan_renderer.hpp"
#include "material.hpp"
#include "utility.hpp"

namespace rendering_engine
{

VulkanMaterialResources::VulkanMaterialResources(VulkanRenderer* renderer)
	:
	mRenderer(renderer)
{}

void VulkanMaterialResources::Initialize(Material * material)
{
	mDescriptorSetLayout = mRenderer->CreateDescriptorSetLayout(material);
	
	const auto matName = material->GetMaterialSettings().materialName;
	boost::filesystem::path matPath = Utility::GetShadersFolderPath() / matName;
	
	auto spvVert = Utility::ReadShaderBinaryFile((matPath / std::string(matName + "_vert.spv")).string());
	auto spvFrag = Utility::ReadShaderBinaryFile((matPath / std::string(matName + "_frag.spv")).string());

	mPipelinePair = mRenderer->CreateGraphicsPipeline(material, mDescriptorSetLayout, spvVert, spvFrag);
}

void VulkanMaterialResources::Shutdown()
{
	auto vulkanLogicalDevice = mRenderer->GetLogicalDevice();
	vkDestroyDescriptorSetLayout(vulkanLogicalDevice, mDescriptorSetLayout, nullptr);

	vkDestroyPipeline(vulkanLogicalDevice, mPipelinePair.second, nullptr);
	vkDestroyPipelineLayout(vulkanLogicalDevice, mPipelinePair.first, nullptr);
}

VkDescriptorSetLayout VulkanMaterialResources::GetDescriptorSetLayout() const
{
	return mDescriptorSetLayout;
}

VkPipelineLayout VulkanMaterialResources::GetPipelineLayout() const
{
	return mPipelinePair.first;
}

VkPipeline VulkanMaterialResources::GetPipeline() const
{
	return mPipelinePair.second;
}

} // namespace rendering_engine