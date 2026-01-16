#include "drawable_component.hpp"

#include "i_renderer.hpp"
#include "i_render_resources.hpp"
#include "model_cache.hpp"
#include "texture_cache.hpp"
#include "material_cache.hpp"
#include "material.hpp"

#include <cstring>

namespace rendering_engine
{

DrawableComponent::DrawableComponent(RenderResourceContext renderContext)
	:
	mRenderContext(renderContext)
{
}

void DrawableComponent::Initialize()
{
	for (auto& renderBatch : mRenderBatches)
	{
		renderBatch.renderResources = std::unique_ptr<IRenderResources>(mRenderContext.renderer->ProvideRenderResources());
		Material* material = mRenderContext.materialCache->GetMaterial(renderBatch.materialName);
		renderBatch.materialParameters = material->PackMaterialParameters();
		MeshDataGpu* meshData = mRenderContext.meshCache->GetMeshResources(renderBatch.meshName).get();

		renderBatch.renderResources->Initialize(material, meshData, mRenderContext.textureCache);
	}
}

void DrawableComponent::Shutdown()
{
	for (auto& renderBatch : mRenderBatches)
	{
		if (renderBatch.renderResources)
		{
			renderBatch.renderResources->Shutdown();
		}
	}
	mRenderBatches.clear();
}

void DrawableComponent::AddRenderBatch(std::string meshName, std::string materialName)
{
	RenderBatch renderBatch;
	renderBatch.meshName = meshName;
	renderBatch.materialName = materialName;

	mRenderBatches.push_back(std::move(renderBatch));
}



} // namespace rendering_engine