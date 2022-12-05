#include "drawable_component.hpp"
#include "utility.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "app_time.hpp"
#include "scene_component.hpp"
#include "camera.hpp"

namespace rendering_engine
{
	DrawableComponent::DrawableComponent()
	{
	}
	DrawableComponent::~DrawableComponent()
	{
	}
	void DrawableComponent::Initialize()
	{
		mSceneComponent = std::make_shared<SceneComponent>();
		mSceneComponent->Initialize();
	}
	void DrawableComponent::Update(float delta)
	{
		mSceneComponent->Update(delta);
	}
	std::shared_ptr<SceneComponent> DrawableComponent::GetSceneComponent()
	{
		return mSceneComponent;
	}
} //rendering_engine