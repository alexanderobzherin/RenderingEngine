#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"
#include "color_space.hpp"
#include "input_event.hpp"
#include "pckeyboard_actor.hpp"
#include "i_application.hpp"

using namespace rendering_engine;

namespace application
{

StartLevel::StartLevel(SceneManager& sceneManager)
	:
	Scene(sceneManager),
	mKeyboard(nullptr)
{}

void StartLevel::Initialize()
{
	Scene::Initialize();

	SetBackgroundColor(ColorSpace::SRGBToLinear(glm::vec3(0.008f, 0.231f, 0.357f)));

	mKeyboard = SpawnActor2D<PCKeyboardActor>();

	const auto& screenSettings = GetSceneManager().GetApplication()->GetScreenSettings();
	const glm::vec2 viewportSize{screenSettings.width, screenSettings.height };
	const glm::vec2 keyboardSize = mKeyboard->GetDimensions();
	const glm::vec2 keyboardMin = mKeyboard->GetLayoutMin();
	const glm::vec2 keyboardCenter = keyboardMin + keyboardSize * 0.5f;

	constexpr float scale = 1.0f;
	mKeyboard->SetScale({ scale, scale });
	mKeyboard->SetPosition(-keyboardCenter * scale);
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);
}
}