/*
#include "drawable_component.hpp"
#include "utility.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "app_time.hpp"
#include "scene_component.hpp"
#include "camera.hpp"
#include "image_data.hpp"

namespace rendering_engine
{
DrawableComponent::DrawableComponent()
	:
	mMeshPath("../Intermediate/Models/TestCube/test_cube.fbx"),
	mColorTexturePath("../Intermediate/Models/TestCube/test_cube_color.png"),
	mSceneComponent(std::make_shared<SceneComponent>())
{
}
DrawableComponent::~DrawableComponent()
{
}
void DrawableComponent::Initialize()
{
	mSceneComponent->Initialize();
	mColorTextureImageData = std::make_shared<ImageData>(mColorTexturePath);
	LoadModel(mMeshPath);
}
void DrawableComponent::Update(float delta)
{
	mSceneComponent->Update(delta);
}

std::shared_ptr<SceneComponent> DrawableComponent::GetSceneComponent()
{
	return mSceneComponent;
}
void DrawableComponent::SetColorTexture(std::string path)
{
	mColorTexturePath = path;
}
void DrawableComponent::SetModelMesh(std::string path)
{
	mMeshPath = path;
}
void DrawableComponent::SetMaterial(std::string materialName)
{
	mMaterialName = materialName;
}
void DrawableComponent::LoadModel(std::string const modelFilepath)
{
	Model model(modelFilepath, true);
	auto const vertices = model.Meshes().at(0)->Vertices();
	auto const indices = model.Meshes().at(0)->Indices();
	auto const texCoord = model.Meshes().at(0)->TextureCoordinates();

	for( int i = 0; i < vertices.size(); ++i )
	{
		Vertex vertex{};

		vertex.pos = vertices[i];
		vertex.texCoord = glm::vec3{ texCoord[0].at(i).x, texCoord[0].at(i).y, texCoord[0].at(i).z };
		vertex.color = { 1.0f, 1.0f, 1.0f };

		mVertices.push_back(vertex);
	}
	 
	mIndices = indices;
}
} //rendering_engine

*/