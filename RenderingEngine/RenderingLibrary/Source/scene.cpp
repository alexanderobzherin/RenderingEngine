#include "scene.hpp"
#include "scene_manager.hpp"
#include "camera_2d.hpp"
#include "camera.hpp"
#include "drawable_2d.hpp"
#include "drawable_3d.hpp"

namespace rendering_engine
{

Scene::Scene(SceneManager& sceneManager)
	:
	mSceneManager(sceneManager)
{}

void Scene::Initialize()
{
}

void Scene::Update(float deltaTime)
{
	if (mActiveCamera2D)
	{
		mActiveCamera2D->Update(deltaTime);
	}
	if (mActiveCamera3D)
	{
		//mActiveCamera3D->Update(deltaTime);
	}

	for (auto& drawable2D : mDrawables2D)
	{
		drawable2D->Update(deltaTime);
	}
	for (auto& drawable3D : mDrawables3D)
	{
		//drawable3D->Update(deltaTime);
	}
}

void Scene::Draw()
{
	if (mActiveCamera2D)
	{
		for (auto& drawable2D : mDrawables2D)
		{
			drawable2D->Draw(*mActiveCamera2D);
		}
	}
	if (mActiveCamera3D)
	{
		for (auto& drawable3D : mDrawables3D)
		{
			drawable3D->Draw(*mActiveCamera3D);
		}
	}
}

void Scene::Shutdown()
{
	for (auto& drawable2D : mDrawables2D)
	{
		drawable2D->Shutdown();
	}
	for (auto& drawable3D : mDrawables3D)
	{
		drawable3D->Shutdown();
	}
}

SceneManager& Scene::GetSceneManager()
{
	return mSceneManager;
}

void Scene::LoadScene(std::string newSceneName)
{
	mSceneManager.LoadScene(newSceneName);
}



} // namespace rendering_engine
