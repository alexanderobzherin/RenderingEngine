#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"
#include "color_space.hpp"
#include "input_event.hpp"
#include "test_button.hpp"

using namespace rendering_engine;

namespace application
{

StartLevel::StartLevel(SceneManager& sceneManager)
	:
	Scene(sceneManager),
	mTestButton(nullptr)
{}

void StartLevel::Initialize()
{
	Scene::Initialize();

	SetBackgroundColor(ColorSpace::SRGBToLinear(glm::vec3(0.008f, 0.231f, 0.357f)));

	mTestButton = SpawnActor2D<TestButton>();

}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
}