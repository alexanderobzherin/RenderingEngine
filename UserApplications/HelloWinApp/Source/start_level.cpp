#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"

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

	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext()));
	mTestCube = static_cast<StaticMesh*>(mDrawables3D.back());
	mTestCube->SetMaterialName("BasicTexture3D");
	mTestCube->SetMeshName("cube");
	mTestCube->Initialize();

	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext()));
	mTestDice1 = static_cast<StaticMesh*>(mDrawables3D.back());
	mTestDice1->SetMaterialName("FlatColorFiltering");
	mTestDice1->SetMeshName("D4");
	mTestDice1->Initialize();
	mTestDice1->SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));

	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext()));
	mTestDice2 = static_cast<StaticMesh*>(mDrawables3D.back());
	mTestDice2->SetMaterialName("FlatColorFiltering");
	mTestDice2->SetMeshName("D4");
	mTestDice2->Initialize();
	mTestDice2->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));

	mDrawables2D.push_back(new Quad2D(mSceneManager.GetRenderResourceContext()));
	mQuad2D = static_cast<Quad2D*>(mDrawables2D.back());
	mQuad2D->SetMaterialName("Quad2D");
	mQuad2D->SetMeshName("Quad2D");

	mQuad2D->Initialize();
	mQuad2D->SetScale(glm::vec2(200, 150));
	mQuad2D->SetPosition(glm::vec2(-200.0f, -150.0f));
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	float newRot = mQuad2D->GetRotation() + (deltaTime * 10);

	mQuad2D->SetRotation(newRot);

	mTestCube->SetRotation(glm::vec3(newRot * 0.5f, 0.0f, newRot * 0.5f));

	mTestDice1->SetRotation(glm::vec3(newRot * 0.5f, -newRot * 0.5f, 0.0f));
	mTestDice2->SetRotation(glm::vec3(-newRot * 0.5f, newRot * 0.5f, 0.0f));
}
}