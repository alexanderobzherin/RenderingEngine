#include "start_level.hpp"
#include "camera.hpp"
#include "camera_2d.hpp"
#include "scene_manager.hpp"
#include "spawn_drawables.hpp"
#include "static_mesh.hpp"
#include "quad_2d.hpp"

#include "cube_cluster_actor.hpp"

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

	mCubeClusterActor = SpawnActor<CubeClusterActor>();
	mCubeClusterActor->SetPosition(glm::vec3(10.0f, 0.0f, 0.0f));
}
void StartLevel::Update(float deltaTime)
{
	Scene::Update(deltaTime);

	mTicking++;
	if (mTicking > mFrequency)
	{
		mTicking = 0;

		mCubeClusterActor->Destroy();

		mCubeClusterActor = SpawnActor<CubeClusterActor>();
		mCubeClusterActor->SetPosition(glm::vec3(10.0f, 0.0f, 0.0f));
	}
}
}