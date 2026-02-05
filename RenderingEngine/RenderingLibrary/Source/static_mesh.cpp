#include "static_mesh.hpp"
#include "drawable_3d.hpp"
#include "camera.hpp"
#include "i_render_resources.hpp"
#include "scene_component.hpp"
#include "scene.hpp"

#include <unordered_map>

namespace rendering_engine
{

StaticMesh::StaticMesh(RenderResourceContext renderContext, Scene& scene, StaticMeshParams params)
	:
	Drawable3D(renderContext, scene),
	mParams(params)
{
}

void StaticMesh::Initialize()
{
	AddRenderBatch(mParams.meshName, mParams.materials.at(0));
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

	for (auto& renderBatch : mRenderBatches)
	{
		renderBatch.renderResources->SubmitResources(transformations, renderBatch.materialParameters);
	}
}

} // namespace rendering_engine
