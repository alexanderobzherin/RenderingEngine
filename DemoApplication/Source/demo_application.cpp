/*

#include "../Include/demo_application.hpp"

#include "app_clock.hpp"
#include "app_time.hpp"
#include "camera.hpp"

namespace demo_application
{
DemoApplication::DemoApplication(char const* appName)
	:
	VulkanRenderer::VulkanRenderer(appName)
{
}

DemoApplication::DemoApplication(int const width, int const height, char const* appName)
	:
	VulkanRenderer::VulkanRenderer(width, height, appName)
{
}

DemoApplication::~DemoApplication()
{
}

void DemoApplication::Shutdown()
{
	rendering_engine::vulkan_renderer::VulkanRenderer::Shutdown();
}

void DemoApplication::Initialize()
{
    VulkanRenderer::Initialize();

    GetDrawableObjects().push_back(std::make_shared<VulkanDrawableComponent>(this));
    GetDrawableObjects().back()->SetColorTexture("../Intermediate/Models/Dice/D4/D4FontTextureSegoeScript.png");
    GetDrawableObjects().back()->SetModelMesh("../Intermediate/Models/Dice/D4/D4.fbx");
    GetDrawableObjects().back()->SetMaterial("FlatColorFiltering");
    //GetDrawableObjects().back()->Initialize();
    GetDrawableObjects().back()->GetSceneComponent()->SetPosition(glm::vec3(4.0f, 0.0f, 0.0f));

    GetDrawableObjects().push_back(std::make_shared<VulkanDrawableComponent>(this));
    //GetDrawableObjects().back()->Initialize();
    GetDrawableObjects().back()->SetMaterial("BasicTexture3D");
    GetDrawableObjects().back()->GetSceneComponent()->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    GetDrawableObjects().push_back(std::make_shared<VulkanDrawableComponent>(this));
    GetDrawableObjects().back()->SetColorTexture("../Intermediate/Models/Dice/D8/D8FontTextureSegoeScript.png");
    GetDrawableObjects().back()->SetModelMesh("../Intermediate/Models/Dice/D8/D8.fbx");
    GetDrawableObjects().back()->SetMaterial("FlatColorFiltering");
    //GetDrawableObjects().back()->Initialize();
    GetDrawableObjects().back()->GetSceneComponent()->SetPosition(glm::vec3(-4.0f, 0.0f, 0.0f));

}

void DemoApplication::Update(float const delta)
{
	rendering_engine::vulkan_renderer::VulkanRenderer::Update(delta);
}

} // demo_application


*/