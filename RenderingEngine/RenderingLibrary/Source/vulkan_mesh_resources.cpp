#include "vulkan_mesh_resources.hpp"
#include "vulkan_renderer.hpp"

#include <cstring>

namespace rendering_engine
{

VulkanMeshResources::VulkanMeshResources(VulkanRenderer* renderer)
	:
	mRenderer(renderer),
	bIsOnGPU(false),
    mVertexBufferSize(0),
    mIndexBufferSize(0)
{
}

void VulkanMeshResources::CreateVertexBuffer(std::vector<Vertex2D> vertices)
{
    auto logicalDevice = mRenderer->GetLogicalDevice();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    mVertexBufferSize = static_cast<size_t>(bufferSize);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);

    memcpy(data, vertices.data(), mVertexBufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mVertexBuffer, mVertexBufferMemory);

    mRenderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

    bIsOnGPU = true;
    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanMeshResources::CreateVertexBuffer(std::vector<VertexPositionColorTexture> vertices)
{
    auto logicalDevice = mRenderer->GetLogicalDevice();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    mVertexBufferSize = static_cast<size_t>(bufferSize);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);

    memcpy(data, vertices.data(), mVertexBufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mVertexBuffer, mVertexBufferMemory);

    mRenderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

    bIsOnGPU = true;
    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanMeshResources::CreateVertexBuffer(std::vector<VertexPositionColorTextureNormalTangent> vertices)
{
    auto logicalDevice = mRenderer->GetLogicalDevice();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    mVertexBufferSize = static_cast<size_t>(bufferSize);
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    
    memcpy(data, vertices.data(), mVertexBufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mVertexBuffer, mVertexBufferMemory);

    mRenderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

    bIsOnGPU = true;
    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

void VulkanMeshResources::CreateIndexBuffer(std::vector<uint32_t> indices)
{
    auto logicalDevice = mRenderer->GetLogicalDevice();

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    mIndexBufferSize = static_cast<size_t>(bufferSize);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), mIndexBufferSize);
    vkUnmapMemory(logicalDevice, stagingBufferMemory);

    mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

    mRenderer->CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
}

bool VulkanMeshResources::IsOnGPU() const
{
    return bIsOnGPU;
}

void VulkanMeshResources::Shutdown()
{
    DeferredItem indexBuffer;
    indexBuffer.type = DeferredType::Buffer;
    indexBuffer.buffer = mIndexBuffer;
    mRenderer->AddDeferredDestroy(indexBuffer);
    mIndexBuffer = VK_NULL_HANDLE;

    DeferredItem indexBufferMemory;
    indexBufferMemory.type = DeferredType::Memory;
    indexBufferMemory.memory = mIndexBufferMemory;
    mRenderer->AddDeferredDestroy(indexBufferMemory);
    mIndexBufferMemory = VK_NULL_HANDLE;

    DeferredItem vertexBuffer;
    vertexBuffer.type = DeferredType::Buffer;
    vertexBuffer.buffer = mVertexBuffer;
    mRenderer->AddDeferredDestroy(vertexBuffer);
    mVertexBuffer = VK_NULL_HANDLE;

    DeferredItem vertexBufferMemory;
    vertexBufferMemory.type = DeferredType::Memory;
    vertexBufferMemory.memory = mVertexBufferMemory;
    mRenderer->AddDeferredDestroy(vertexBufferMemory);
    mVertexBufferMemory = VK_NULL_HANDLE;

    bIsOnGPU = false;
}

VkBuffer VulkanMeshResources::GetVertexBuffer() const
{
    return mVertexBuffer;
}

VkBuffer VulkanMeshResources::GetIndexBuffer() const
{
    return mIndexBuffer;
}

size_t VulkanMeshResources::GetVertexBufferSize() const
{
    return mVertexBufferSize;
}

size_t VulkanMeshResources::GetIndexBufferSize() const
{
    return mIndexBufferSize;
}

} // namespace rendering_engine