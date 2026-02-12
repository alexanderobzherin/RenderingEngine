#include "cube_cluster_actor.hpp"

#include "create_actor_subobject.hpp"
#include "scene_component.hpp"
#include "static_mesh.hpp"

using namespace rendering_engine;

namespace application
{
CubeClusterActor::CubeClusterActor(Scene& scene)
	:
	Actor(scene)
{}
void CubeClusterActor::Initialize()
{
	Actor::Initialize();

	bUpdateOnTick = true;

	StaticMeshParams cubeMeshParams;
	cubeMeshParams.meshName = "cube";
	cubeMeshParams.materials = { { 0U, "BasicTexture3D" } };
	mCenterCubeMesh = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCenterCubeMesh->GetTransform().AttachTo(&mRootComponent);
	mCenterCubeMesh->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));

	mCubeMesh_1 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_1->GetTransform().AttachTo(&mRootComponent);
	mCubeMesh_1->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));

	mCubeMesh_2 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_2->GetTransform().AttachTo(&mRootComponent);
	mCubeMesh_2->SetPosition(glm::vec3(0.0f, -3.0f, 0.0f));

	mCubeMesh_3 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_3->GetTransform().AttachTo(&mRootComponent);
	mCubeMesh_3->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

	mCubeMesh_4 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_4->GetTransform().AttachTo(&mRootComponent);
	mCubeMesh_4->SetPosition(glm::vec3(0.0f, 0.0f, -3.0f));

	mCubeMesh_5 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_5->GetTransform().AttachTo(&mRootComponent);
	mCubeMesh_5->SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));

	mCubeMesh_6 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_6->GetTransform().AttachTo(&mRootComponent);
	mCubeMesh_6->SetPosition(glm::vec3(-3.0f, 0.0f, 0.0f));

	mCubeMesh_5_1 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_5_1->GetTransform().AttachTo(&mCubeMesh_5->GetTransform());
	mCubeMesh_5_1->SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));

	mCubeMesh_6_1 = CreateSubobject<StaticMesh>(cubeMeshParams);
	mCubeMesh_6_1->GetTransform().AttachTo(&mCubeMesh_6->GetTransform());
	mCubeMesh_6_1->SetPosition(glm::vec3(-3.0f, 0.0f, 0.0f));
	
	SetRotation(glm::vec3(65.0f, 25.0f, 45.0f));
	const glm::vec3 oldRotation = GetRotation();

}

void CubeClusterActor::Update(float deltaTime)
{
	Actor::Update(deltaTime);

	const glm::vec3 oldRotation = GetRotation();
	const glm::vec3 newRotation = glm::vec3(oldRotation.x + mAngularVelocity,
		0.0f,
		oldRotation.z + mAngularVelocity);
	SetRotation(newRotation);

	const glm::vec3 oldPosition = GetPosition();
	const glm::vec3 newPosition = oldPosition + (mMovementDirection * mSpeed);
	SetPosition(newPosition);

	const glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 distance = startPos - newPosition;
	const float distanceFloat = glm::length(distance);
	if (distanceFloat > 16.0f)
	{
		mMovementDirection *= -1.0f;
	}
}

} // namespace application
