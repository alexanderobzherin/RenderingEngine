#include "scene.hpp"
#include "scene_manager.hpp"
#include "camera_2d.hpp"
#include "camera.hpp"
#include "drawable_2d.hpp"
#include "drawable_3d.hpp"
#include "drawable_component.hpp"
#include "actor.hpp"
#include "actor_2d.hpp"
#include "logger.hpp"

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
	// Update cameras
	if (mActiveCamera2D)
	{
		mActiveCamera2D->Update(deltaTime);
	}
	if (mActiveCamera3D)
	{
		// TO DO - uncomment
		//mActiveCamera3D->Update(deltaTime);
	}

	for (auto& actor : mActors)
	{
		if (!actor->IsPendingDestroy())
			actor->Update(deltaTime);
	}
	for (auto& actor2D : mActors2D)
	{
		if (!actor2D->IsPendingDestroy())
			actor2D->Update(deltaTime);
	}

	for (auto& drawable2D : mDrawables2D)
	{
		drawable2D->Update(deltaTime);
	}
	for (auto& drawable3D : mDrawables3D)
	{
		drawable3D->Update(deltaTime);
	}

	FlushDestroyed();
}

void Scene::Draw()
{
	if (mActiveCamera3D)
	{
		for (auto& drawable3D : mDrawables3D)
		{
			drawable3D->Draw(*mActiveCamera3D);
		}
	}

	if (mActiveCamera2D)
	{
		for (auto& drawable2D : mDrawables2D)
		{
			drawable2D->Draw(*mActiveCamera2D);
		}
	}
}

void Scene::Shutdown()
{
    // Finish any queued destroys first (so we don't double-delete later)
    FlushDestroyed();

    // Destroy drawables FIRST (they may reference actor transforms as parents)
    for (auto* d : mDrawables2D) { if (d) { d->Shutdown(); delete d; } }
    mDrawables2D.clear();

    for (auto* d : mDrawables3D) { if (d) { d->Shutdown(); delete d; } }
    mDrawables3D.clear();

    // Now destroy actors
    for (auto* a : mActors) { if (a) { a->Shutdown(); delete a; } }
    mActors.clear();

	for (auto* a : mActors2D) { if (a) { a->Shutdown(); delete a; } }
	mActors2D.clear();

    mPendingDestroy2D.clear();
    mPendingDestroy3D.clear();
    mPendingDestroyActors.clear();
	mPendingDestroyActors2D.clear();
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

void Scene::DestroyActor(Actor* actor)
{
	mPendingDestroyActors.push_back(actor);
}

void Scene::DestroyActor(Actor2D* actor2D)
{
	mPendingDestroyActors2D.push_back(actor2D);
}

void Scene::FlushDestroyed()
{
	for (auto* d : mPendingDestroy3D)
	{
		auto it = std::find(mDrawables3D.begin(), mDrawables3D.end(), d);
		if (it != mDrawables3D.end())
		{
			(*it)->Shutdown();
			delete* it;
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
			delete* it;
			mDrawables2D.erase(it);
		}
	}
	mPendingDestroy2D.clear();

	for (auto* actor : mPendingDestroyActors)
	{
		auto it = std::find(mActors.begin(), mActors.end(), actor);
		if (it != mActors.end())
		{
			(*it)->Shutdown();
			delete* it;
			mActors.erase(it);
		}
	}
	mPendingDestroyActors.clear();

	for (auto* actor2D : mPendingDestroyActors2D)
	{
		auto it = std::find(mActors2D.begin(), mActors2D.end(), actor2D);
		if (it != mActors2D.end())
		{
			(*it)->Shutdown();
			delete* it;
			mActors2D.erase(it);
		}
	}
	mPendingDestroyActors2D.clear();
}

} // namespace rendering_engine
