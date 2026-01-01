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
	mRenderContext(renderContext),
	mMaterial(nullptr),
	mMeshData(nullptr)
{
	mRenderResources = std::unique_ptr<IRenderResources>(mRenderContext.renderer->ProvideRenderResources());
}

void DrawableComponent::Initialize()
{
	mMaterial = mRenderContext.materialCache->GetMaterial(mMaterialName);
	mMaterialParameters = mMaterial->PackMaterialParameters();
	mMeshData = mRenderContext.meshCache->GetMeshResources(mMeshName).get();

	mRenderResources->Initialize(mMaterial, mMeshData, mRenderContext.textureCache);
}

void DrawableComponent::Shutdown()
{
	if (mRenderResources)
	{
		mRenderResources->Shutdown();
	}
}

void DrawableComponent::SetMaterialVec4(const std::string& name, const glm::vec4& value)
{
	for (const auto& entry : mMaterialParameters.layout)
	{
		if (entry.name == name && entry.type == MaterialParameterLayoutEntry::Type::Vec4)
		{
			std::memcpy(
				mMaterialParameters.buffer.data() + entry.offset,
				&value,
				sizeof(glm::vec4)
			);
			return;
		}
	}
}

void DrawableComponent::SetMaterialName(const std::string& materialName)
{
	mMaterialName = materialName;
}
void DrawableComponent::SetMeshName(const std::string& meshName)
{
	mMeshName = meshName;
}

}