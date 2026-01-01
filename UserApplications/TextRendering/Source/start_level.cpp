#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"
#include "text_block_2d.hpp"
#include "text_renderer.hpp"

using namespace rendering_engine;

namespace application
{

StartLevel::StartLevel(SceneManager& sceneManager)
	:
	Scene(sceneManager)
{}

void StartLevel::Initialize()
{
	Scene::Initialize();

	mActiveCamera3D = std::make_shared<Camera>(*mSceneManager.GetApplication());
	mActiveCamera3D->Initialize();

	mActiveCamera2D = std::make_shared<Camera2D>(*mSceneManager.GetApplication());
	mActiveCamera2D->Initialize();

	mDrawables2D.push_back(new Quad2D(mSceneManager.GetRenderResourceContext()));
	mQuad2D = static_cast<Quad2D*>(mDrawables2D.back());
	mQuad2D->SetMeshName("Quad2D");
	mQuad2D->SetMaterialName("Quad2D");

	mQuad2D->Initialize();
	mQuad2D->SetScale(glm::vec2(200, 150));
	mQuad2D->SetPosition(glm::vec2(200.0f, -150.0f));

	const std::string fontName = "RobotoMono-Bold";
	mDrawables2D.push_back(new TextBlock2D(mSceneManager.GetTextRenderer(), fontName));
	mTextBlock = static_cast<TextBlock2D*>(mDrawables2D.back());
	mTextBlock->DrawFontAtlas();
	mTextBlock->Initialize();
	mTextBlock->SetPosition(glm::vec2(-200.0f, -150.0f));
	mTextBlock->SetTextColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
}