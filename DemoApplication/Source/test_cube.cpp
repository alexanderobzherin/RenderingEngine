#include "test_cube.hpp"
#include "scene.hpp"
#include "static_mesh.hpp"

namespace demo_application
{

TestCube::TestCube(Scene& scene)
	:
	Actor(scene)
{}

void TestCube::Initialize()
{
	mMesh = std::make_unique<StaticMesh>(GetRenderContext());

	mMesh->SetMaterialName("BasicTexture3D");
	mMesh->SetMeshName("cube");
	mMesh->Initialize();

	mMesh->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	mMesh->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
	mMesh->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
}

void TestCube::Update(float deltaTime)
{
	mMesh->Update(deltaTime);
}

void TestCube::Draw(const Camera& camera)
{
	mMesh->Draw(camera);
}

} // namespace demo_application
