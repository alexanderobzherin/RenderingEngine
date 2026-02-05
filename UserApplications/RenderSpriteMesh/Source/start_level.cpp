#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"

#include "spawn_drawables.hpp"

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

	StaticMeshParams param1;
	param1.meshName = "cube";
	param1.materials = { { 0U, "BasicTexture3D" } };
	mTestCube = Spawn<StaticMesh>(param1);

	StaticMeshParams param2;
	param2.meshName = "D4";
	param2.materials = { { 0U, "FlatColorFiltering" } };
	mTestDice1 = Spawn<StaticMesh>(param2);
	mTestDice1->SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));

	mTestDice2 = Spawn<StaticMesh>(param2);
	mTestDice2->SetPosition(glm::vec3(0.0f, 5.0f, 3.0f));

	const std::string textureName = "PNG_transparency_demonstration_1";
	mSprite2D = Spawn<Sprite2D>(textureName);
	mSprite2D->SetPosition(glm::vec2(-300.0f, -200.0f));
	mSprite2D->SetSpriteScale(0.2f);
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