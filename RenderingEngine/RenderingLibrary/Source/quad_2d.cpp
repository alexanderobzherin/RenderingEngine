#include "quad_2d.hpp"
#include "camera_2d.hpp"
#include "i_render_resources.hpp"
#include "scene_component_2d.hpp"
#include "scene.hpp"

namespace rendering_engine
{

Quad2D::Quad2D(RenderResourceContext renderContext, Scene& scene)
	:
	Drawable2D(renderContext, scene)
{}

void Quad2D::Initialize()
{
	Drawable2D::Initialize();
}

void Quad2D::Update(float deltaTime)
{
	Drawable2D::Update(deltaTime);
}

void Quad2D::Draw(const Camera2D& camera)
{
	Transformations2D transformations;
	transformations.model = GetTransform().GetWorldMatrix();
	transformations.view = camera.GetWorldView();
	transformations.proj = camera.GetProjectionMatrix();

	for (auto& renderBatch : mRenderBatches)
	{
		renderBatch.renderResources->SubmitResources(transformations, renderBatch.materialParameters);
	}
}

} // namespace rendering_engine
