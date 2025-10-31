#include <vulkan/vulkan.h>
#include "test_sprite2D.hpp"
#include "vulkan_renderer.hpp"
#include "utility.hpp"

namespace rendering_engine
{
    TestSprite2D::TestSprite2D() {}

    void TestSprite2D::Initialize(VulkanRenderer* renderer)
    {
        mRenderer = renderer;

        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateGraphicsPipeline();

    }

    void TestSprite2D::Update(float deltaTime)
    {
    }

    void TestSprite2D::Draw()
    {
        auto commandBuffers = mRenderer->GetComandBuffers();
        auto frame = mRenderer->GetCurrentFrame();

        vkCmdBindPipeline(commandBuffers[frame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
        
        /*If dynamicState in pipeline is used then vkCmdSetViewport and vkCmdSetScissor per draw call*/
        //VkViewport viewport{};
        //viewport.x = 0.0f;
        //viewport.y = 0.0f;
        //viewport.width = static_cast<float>(mRenderer->GetSwapChainExtent().width);
        //viewport.height = static_cast<float>(mRenderer->GetSwapChainExtent().height);
        //viewport.minDepth = 0.0f;
        //viewport.maxDepth = 1.0f;
        //vkCmdSetViewport(commandBuffers[frame], 0, 1, &viewport);

        //VkRect2D scissor{};
        //scissor.offset = { 0, 0 };
        //scissor.extent = mRenderer->GetSwapChainExtent();
        //vkCmdSetScissor(commandBuffers[frame], 0, 1, &scissor);

        VkBuffer vertexBuffers[] = { mVertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffers[frame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[frame], mIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdDrawIndexed(commandBuffers[frame], static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);
    }

    void TestSprite2D::Shutdown()
    {
        auto vulkanLogicalDevice = mRenderer->GetLogicalDevice();
        vkDestroyPipeline(vulkanLogicalDevice, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(vulkanLogicalDevice, pipelineLayout, nullptr);

        vkDestroyBuffer(vulkanLogicalDevice, mIndexBuffer, nullptr);
        vkFreeMemory(vulkanLogicalDevice, mIndexBufferMemory, nullptr);

        vkDestroyBuffer(vulkanLogicalDevice, mVertexBuffer, nullptr);
        vkFreeMemory(vulkanLogicalDevice, mVertexBufferMemory, nullptr);
    }

    void TestSprite2D::CreateVertexBuffer()
    {
        auto logicalDevice = mRenderer->GetLogicalDevice();

        VkDeviceSize bufferSize = sizeof(mVertices[0]) * mVertices.size();
        mVertexBufferSize = static_cast<size_t>(bufferSize);
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);

        memcpy(data, mVertices.data(), mVertexBufferSize);
        vkUnmapMemory(logicalDevice, stagingBufferMemory);
        mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mVertexBuffer, mVertexBufferMemory);

        mRenderer->CopyBuffer(stagingBuffer, mVertexBuffer, bufferSize);

        vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
    }

    void TestSprite2D::CreateIndexBuffer()
    {
        auto logicalDevice = mRenderer->GetLogicalDevice();

        VkDeviceSize bufferSize = sizeof(mIndices[0]) * mIndices.size();
        mIndexBufferSize = static_cast<size_t>(bufferSize);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, mIndices.data(), mIndexBufferSize);
        vkUnmapMemory(logicalDevice, stagingBufferMemory);

        mRenderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mIndexBuffer, mIndexBufferMemory);

        mRenderer->CopyBuffer(stagingBuffer, mIndexBuffer, bufferSize);

        vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);
    }

    void TestSprite2D::CreateGraphicsPipeline()
    {
        auto spvVert = Utility::ReadShaderBinaryFile("../Intermediate/Shaders/TestSprite2D/TestSprite2D_vert.spv");
        auto spvFrag = Utility::ReadShaderBinaryFile("../Intermediate/Shaders/TestSprite2D/TestSprite2D_frag.spv");

        auto logicalDevice = mRenderer->GetLogicalDevice();
        auto CreateShaderModule = [&](std::vector<char>& code)
        {
            VkShaderModuleCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = code.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

            VkShaderModule shaderModule;
            if (vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create shader module!");
            }

            return shaderModule;
        };

        VkShaderModule vertShaderModule = CreateShaderModule(spvVert);
        VkShaderModule fragShaderModule = CreateShaderModule(spvFrag);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::array<VkVertexInputAttributeDescription, 2> attribDescs{};
        attribDescs[0].binding = 0;
        attribDescs[0].location = 0; // position
        attribDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribDescs[0].offset = offsetof(Vertex, pos);

        attribDescs[1].binding = 0;
        attribDescs[1].location = 1; // color
        attribDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribDescs[1].offset = offsetof(Vertex, color);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribDescs.size());
        vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)mRenderer->GetSwapChainExtent().width;
        viewport.height = (float)mRenderer->GetSwapChainExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = mRenderer->GetSwapChainExtent();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = mRenderer->GetMSAASamples();

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        // To use dynamicState, assign reference to it in pipelineInfo.pDynamicState.
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        //depthStencil.minDepthBounds = 0.0f; // Optional
        //depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        //depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr;// &dynamicState;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = mRenderer->GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
        vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
    }


}