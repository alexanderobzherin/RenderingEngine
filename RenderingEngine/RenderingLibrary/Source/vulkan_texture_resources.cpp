#include "vulkan_texture_resources.hpp"
#include "vulkan_renderer.hpp"

namespace rendering_engine
{

VulkanTextureResources::VulkanTextureResources(VulkanRenderer* renderer)
	:
    bIsTextureLoadedInGPU(false),
	mRenderer(renderer),
	mImageData(nullptr),
    mGpuMemorySize(0)
{}

void VulkanTextureResources::LoadToGPU(const ImageData & data)
{
	mImageData = &data;

    CreateTextureImage();
    CreateTextureImageView();
    CreateTextureSampler();

    bIsTextureLoadedInGPU = true;
}

void VulkanTextureResources::ReleaseFromGPU()
{
    vkDestroySampler(mRenderer->GetLogicalDevice(), mTextureSampler, nullptr);
    vkDestroyImageView(mRenderer->GetLogicalDevice(), mTextureImageView, nullptr);
    vkDestroyImage(mRenderer->GetLogicalDevice(), mTextureImage, nullptr);
    vkFreeMemory(mRenderer->GetLogicalDevice(), mTextureImageMemory, nullptr);

    bIsTextureLoadedInGPU = false;
}

bool VulkanTextureResources::IsTextureLoadedInGPU()
{
    return bIsTextureLoadedInGPU;
}

size_t VulkanTextureResources::GetSizeInGPUBytes() const
{
    return static_cast<size_t>(mGpuMemorySize);
}


void VulkanTextureResources::CreateTextureImage()
{
    if (!mImageData)
    {
        throw std::runtime_error("ImageData is not initialized!");
    }

    unsigned int const width = mImageData->GetWidth();
    unsigned int const height = mImageData->GetHeight();

    mMipmapLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;
    auto const pixelVector = mImageData->GetImageDataRGBA();

    VkDeviceSize imageSize = width * height * 4;

    if (pixelVector.size() == 0)
    {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    mRenderer->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    unsigned char* pixels = new unsigned char[width * height * 4];
    std::copy(pixelVector.begin(), pixelVector.end(), pixels);
    void* data;
    vkMapMemory(mRenderer->GetLogicalDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(mRenderer->GetLogicalDevice(), stagingBufferMemory);
    delete[] pixels;

    CreateVulkanImage(width, height, mMipmapLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mTextureImage, mTextureImageMemory);

    mRenderer->TransitionImageLayout(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mMipmapLevels);
    mRenderer->CopyBufferToImage(stagingBuffer, mTextureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    vkDestroyBuffer(mRenderer->GetLogicalDevice(), stagingBuffer, nullptr);
    vkFreeMemory(mRenderer->GetLogicalDevice(), stagingBufferMemory, nullptr);

    mRenderer->GenerateMipmaps(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, width, height, mMipmapLevels);
}

void VulkanTextureResources::CreateVulkanImage(uint32_t width, uint32_t height, std::uint32_t mipmapLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(width);
    imageInfo.extent.height = static_cast<uint32_t>(height);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipmapLevels;
    imageInfo.arrayLayers = 1;

    imageInfo.format = format;
    imageInfo.tiling = tiling;

    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    imageInfo.usage = usage;

    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.samples = numSamples;
    imageInfo.flags = 0; // Optional

    if (vkCreateImage(mRenderer->GetLogicalDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(mRenderer->GetLogicalDevice(), image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = mRenderer->FindMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(mRenderer->GetLogicalDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    mGpuMemorySize = allocInfo.allocationSize;

    vkBindImageMemory(mRenderer->GetLogicalDevice(), image, imageMemory, 0);
    allocInfo.allocationSize;
}

void VulkanTextureResources::CreateTextureImageView()
{
    mTextureImageView = CreateImageView(mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mMipmapLevels);
}

VkImageView VulkanTextureResources::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, std::uint32_t mipmapLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipmapLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(mRenderer->GetLogicalDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

void VulkanTextureResources::CreateTextureSampler()
{
    VkSamplerCreateInfo samplerInfo{};

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(mRenderer->GetPhysicalDevice(), &properties);

    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    if (mRenderer->GetPhysDevSupportedFeatures().samplerAnisotropy)
    {
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    }
    else
    {
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1.0f;
    }

    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mMipmapLevels - 1);

    if (vkCreateSampler(mRenderer->GetLogicalDevice(), &samplerInfo, nullptr, &mTextureSampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

}