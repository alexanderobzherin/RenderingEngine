#include "static_mesh.hpp"
#include "drawable_3d.hpp"
#include "camera.hpp"
#include "i_render_resources.hpp"
#include "scene_component.hpp"

namespace rendering_engine
{

StaticMesh::StaticMesh(RenderResourceContext renderContext)
	:
	Drawable3D(renderContext)
{
}

void StaticMesh::Initialize()
{
	Drawable3D::Initialize();
}

void StaticMesh::Update(float deltaTime)
{
	Drawable3D::Update(deltaTime);
}

void StaticMesh::Draw(const Camera& camera)
{
	Transformations3D transformations;
	transformations.model = mSceneComponent.GetWorldMatrix();
	transformations.view = camera.ViewMatrix();
	transformations.proj = camera.ProjectionMatrix();

	mRenderResources->SubmitResources(transformations, mMaterialParameters);
}

} // namespace rendering_engine
