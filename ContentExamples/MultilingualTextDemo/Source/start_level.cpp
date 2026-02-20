#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"
#include "multilingual_text_actor.hpp"
#include "color_space.hpp"

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

	SetBackgroundColor(ColorSpace::SRGBToLinear(glm::vec3(0.0f, 0.118f, 0.227f)));
	mMultilingualTextActor = SpawnActor2D<MultilingualTextActor>();
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
}