#include "scene.hpp"
#include "scene_manager.hpp"
#include "camera_2d.hpp"
#include "camera.hpp"
#include "drawable_2d.hpp"
#include "drawable_3d.hpp"
#include "drawable_component.hpp"

namespace rendering_engine
{


Scene::Scene(SceneManager& sceneManager)
	:
	mSceneManager(sceneManager)
{}

void Scene::Initialize()
{
	mActiveCamera3D = std::make_shared<Camera>(*mSceneManager.GetApplication());
	mActiveCamera3D->Initialize();

	mActiveCamera2D = std::make_shared<Camera2D>(*mSceneManager.GetApplication());
	mActiveCamera2D->Initialize();
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
	FlushDestroyed();
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
	mDrawables2D.clear();
	mDrawables3D.clear();
}

SceneManager& Scene::GetSceneManager()
{
	return mSceneManager;
}

void Scene::LoadScene(std::string newSceneName)
{
	mSceneManager.LoadScene(newSceneName);
}

std::shared_ptr<Camera> Scene::GetActiveCamera3D()
{
	return mActiveCamera3D;
}

std::shared_ptr<Camera2D> Scene::GetActiveCamera2D()
{
	return mActiveCamera2D;
}

void Scene::DestroyDrawable3D(Drawable3D* drawable3D)
{
	mPendingDestroy3D.push_back(drawable3D);
}

void Scene::DestroyDrawable2D(Drawable2D* drawable2D)
{
	mPendingDestroy2D.push_back(drawable2D);
}

void Scene::FlushDestroyed()
{
	for (auto* d : mPendingDestroy3D)
	{
		auto it = std::find(mDrawables3D.begin(), mDrawables3D.end(), d);
		if (it != mDrawables3D.end())
		{
			(*it)->Shutdown();
			mDrawables3D.erase(it);
		}
	}
	mPendingDestroy3D.clear();

	for (auto* d : mPendingDestroy2D)
	{
		auto it = std::find(mDrawables2D.begin(), mDrawables2D.end(), d);
		if (it != mDrawables2D.end())
		{
			(*it)->Shutdown();
			mDrawables2D.erase(it);
		}
	}
	mPendingDestroy2D.clear();
}

} // namespace rendering_engine
