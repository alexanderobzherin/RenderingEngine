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
	Shutdown(); 

	mDescriptorSetLayout = mRenderer->CreateDescriptorSetLayout(material);
	
	std::string matName;
	if (material->GetMaterialSettings().parentMaterialName.empty())
	{
		matName = material->GetMaterialSettings().materialName;
	}
	else
	{
		matName = material->GetMaterialSettings().parentMaterialName;
	}

	std::vector<char> spvVert;
	std::vector<char> spvFrag;

	if (Utility::IsPackageProvided())
	{
		const auto& entries = Utility::GetPackEntries();
		std::string materialEntry = "Shaders/" + matName;

		std::vector<uint8_t> binaryFileDataVert = Utility::ReadPackedFile(materialEntry + "/" + std::string(matName + "_vert.spv"));
		std::vector<uint8_t> binaryFileDataFrag = Utility::ReadPackedFile(materialEntry + "/" + std::string(matName + "_frag.spv"));

		spvVert.assign(binaryFileDataVert.begin(), binaryFileDataVert.end());
		spvFrag.assign(binaryFileDataFrag.begin(), binaryFileDataFrag.end());
	}
	else
	{
		boost::filesystem::path matPath = Utility::GetShadersFolderPath() / matName;
	
		spvVert = Utility::ReadShaderBinaryFile((matPath / std::string(matName + "_vert.spv")).string());
		spvFrag = Utility::ReadShaderBinaryFile((matPath / std::string(matName + "_frag.spv")).string());
	}

	mPipelinePair = mRenderer->CreateGraphicsPipeline(material, mDescriptorSetLayout, spvVert, spvFrag);
}

void VulkanMaterialResources::Shutdown()
{
    if (!mRenderer)
        return;

    VkDevice device = mRenderer->GetLogicalDevice(); // or GetDevice()

    // Pipeline first
    if (mPipelinePair.second != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(device, mPipelinePair.second, nullptr);
        mPipelinePair.second = VK_NULL_HANDLE;
    }

    // Then pipeline layout
    if (mPipelinePair.first != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(device, mPipelinePair.first, nullptr);
        mPipelinePair.first = VK_NULL_HANDLE;
    }

    // Then descriptor set layout
    if (mDescriptorSetLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
        mDescriptorSetLayout = VK_NULL_HANDLE;
    }
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