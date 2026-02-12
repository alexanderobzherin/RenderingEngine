#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"

#include "drawable_component.hpp"

#include <random>

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

	// Generate random positions in visible viewport
	// 	  For 3D objects:
	//          0.0   [-5; 5] [-3; 3]
	//	 For 2D objects:
	//        [-300; 300] [-200; 200]

	auto SpawnCube = [&]() {
		StaticMeshParams param1;
		param1.meshName = "cube";
		param1.materials = { { 0U, "BasicTexture3D" } };
		StaticMesh* cube = Spawn<StaticMesh>(param1);
		
		const float y = static_cast<float>(GenerateNumber(-5, 5));
		const float z = static_cast<float>(GenerateNumber(-3, 3));
		cube->SetPosition(glm::vec3(0.0f, y, z));
		return cube;
	};
	mDrawableSpawners.push_back(SpawnCube);

	auto SpawnDice = [&]() {
		StaticMeshParams param2;
		param2.meshName = "D4";
		param2.materials = { { 0U, "FlatColorFiltering" } };
		StaticMesh* dice = Spawn<StaticMesh>(param2);
		const float y = static_cast<float>(GenerateNumber(-5, 5));
		const float z = static_cast<float>(GenerateNumber(-3, 3));
		dice->SetPosition(glm::vec3(0.0f, y, z));
		return dice;
	};
	mDrawableSpawners.push_back(SpawnDice);

	auto SpawnSprite = [&]() {
		const std::string textureName = "PNG_transparency_demonstration_1";
		Sprite2D* sprite = Spawn<Sprite2D>(textureName);
		sprite->SetSpriteScale(0.2f);
		const int x= GenerateNumber(-6, 6); // * 50.0f
		const int y = GenerateNumber(-5, 5); // * 40.0f
		sprite->SetPosition(glm::vec2(50.0f * x, 40.0f * y));
		return sprite;
	};
	mDrawableSpawners.push_back(SpawnSprite);

	auto SpawnTextBlock2D = [&]() {
		TextBlock2D::Properties tbProp;
		tbProp.fontSize = 18;
		TextBlock2D* textBlock2D = Spawn<TextBlock2D>(tbProp);
		textBlock2D->SetText("Random Text Block");
		const int R255 = GenerateNumber(0, 255);
		const int G255 = GenerateNumber(0, 255);
		const int B255 = GenerateNumber(0, 255);
		const float r = static_cast<float>(R255) / 255.0f;
		const float g = static_cast<float>(G255) / 255.0f;
		const float b = static_cast<float>(B255) / 255.0f;

		textBlock2D->SetTextColor(glm::vec4(r, g, b, 1.0f));

		const int x = GenerateNumber(-6, 6); // * 50.0f
		const int y = GenerateNumber(-5, 5); // * 40.0f
		textBlock2D->SetPosition(glm::vec2(50.0f * x, 40.0f * y));
		return textBlock2D;
	};
	mDrawableSpawners.push_back(SpawnTextBlock2D);



	for (int i = 0; i < mInnitNumberOfObjects; i++)
	{
		SpawnRandomObject();
	}
	
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	mTicking++;
	if (mTicking > mFrequency)
	{
		mTicking = 0;
		
		SpawnRandomObject();
		DestroyRandomObject();
	}
}
int StartLevel::GenerateNumber(int min, int max)
{
	std::random_device random_device;
	std::default_random_engine random_generator(random_device());
	std::uniform_int_distribution<int> prob_distribution(min, max);
	unsigned int const number = prob_distribution(random_generator);
	return number;
}
void StartLevel::SpawnRandomObject()
{
	if (mDrawableSpawners.empty())
		return;

	const int min = 0;
	const int max = mDrawableSpawners.size() - 1;
	size_t random = static_cast<size_t>(GenerateNumber(min, max));
	mHandledDrawables.push_back(mDrawableSpawners[random]());
}
void StartLevel::DestroyRandomObject()
{
	if (mHandledDrawables.empty())
		return;

	const int min = 0;
	const int max = mHandledDrawables.size() - 1;
	size_t random = static_cast<size_t>(GenerateNumber(min, max));
	auto lucky = mHandledDrawables[random];
	lucky->Destroy();
	mHandledDrawables.erase(mHandledDrawables.begin() + random);
}
}