#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "static_mesh.hpp"
#include "sprite_2d.hpp"

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

	StaticMeshParams param1;
	param1.meshName = "cube";
	param1.materials = { { 0U, "BasicTexture3D" } };
	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext(), param1));
	mTestCube = static_cast<StaticMesh*>(mDrawables3D.back());
	mTestCube->Initialize();

	StaticMeshParams param2;
	param2.meshName = "D4";
	param2.materials = { { 0U, "FlatColorFiltering" } };
	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext(), param2));
	mTestDice1 = static_cast<StaticMesh*>(mDrawables3D.back());
	mTestDice1->Initialize();
	mTestDice1->SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));

	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext(), param2));
	mTestDice2 = static_cast<StaticMesh*>(mDrawables3D.back());
	mTestDice2->Initialize();
	mTestDice2->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));

	const std::string textureName = "PNG_transparency_demonstration_1";
	mDrawables2D.push_back(new Sprite2D(mSceneManager.GetRenderResourceContext(), textureName));
	mSprite2D = static_cast<Sprite2D*>(mDrawables2D.back());

	mSprite2D->Initialize();
	mSprite2D->SetPosition(glm::vec2(-200.0f, -150.0f));
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	float newRot = mSprite2D->GetRotation() + (deltaTime * 10);

	mSprite2D->SetRotation(newRot);

	mTestCube->SetRotation(glm::vec3(newRot * 0.5f, 0.0f, newRot * 0.5f));

	mTestDice1->SetRotation(glm::vec3(newRot * 0.5f, -newRot * 0.5f, 0.0f));
	mTestDice2->SetRotation(glm::vec3(-newRot * 0.5f, newRot * 0.5f, 0.0f));
}
}