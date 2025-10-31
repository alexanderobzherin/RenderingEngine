#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

namespace rendering_engine
{
class VulkanRenderer;
class TestTriangle
{
public:
	TestTriangle();
	~TestTriangle() = default;
	
	void Initialize(VulkanRenderer* renderer);
	void Update(float deltaTime);
	void Draw();

	void Shutdown();

protected:
	void CreateGraphicsPipeline();


private:
	VulkanRenderer* mRenderer;

	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
};
}