#include "sprite_2d.hpp"
#include "camera_2d.hpp"
#include "i_render_resources.hpp"
#include "scene_component_2d.hpp"
#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "image_data.hpp"
#include "material_cache.hpp"
#include "material.hpp"
#include "material_types.hpp"
#include "scene.hpp"

namespace rendering_engine
{
Sprite2D::Sprite2D(RenderResourceContext renderContext, Scene& scene, std::string textureName)
	:
	Drawable2D(renderContext, scene),
	mTextureName(textureName)
{}

void Sprite2D::Initialize()
{
	auto texture = mRenderContext.textureCache->GetTextureResources(mTextureName);
	const float width = static_cast<float>(texture->GetCpuImageData().GetWidth());
	const float height = static_cast<float>(texture->GetCpuImageData().GetHeight());

	const std::string materialName = std::string{ "Sprite2D_" } + mTextureName + "Mat";

	MaterialSettings materialSettings;
	materialSettings.parentMaterialName = std::string{ "Quad2D" };
	materialSettings.materialName = materialName;
	materialSettings.materialDomain = MaterialDomain::Sprite2D;
	materialSettings.shadingModel = ShadingModel::Unlit;
	materialSettings.blendMode = BlendMode::Opaque;

	auto materialCache = mRenderContext.materialCache;
	materialCache->AddMaterial(materialSettings);
	Material* material = materialCache->GetMaterial(materialName);
	material->AddTexture(mTextureName);
	material->InitializeRenderResources();

	const std::string meshName("Quad2D");
	AddRenderBatch(meshName, materialName);

	Drawable2D::Initialize();
	mTextureRespectiveScale = glm::vec2(width, height);
	Drawable2D::SetScale(mTextureRespectiveScale);
}

void Sprite2D::Update(float deltaTime)
{
	Drawable2D::Update(deltaTime);
}

void Sprite2D::Draw(const Camera2D& camera)
{
	Transformations2D transformations;
	transformations.model = GetTransform().GetModelMatrix();
	transformations.view = camera.GetWorldView();
	transformations.proj = camera.GetProjectionMatrix();

	for (auto& renderBatch : mRenderBatches)
	{
		renderBatch.renderResources->SubmitResources(transformations, renderBatch.materialParameters);
	}
}

void Sprite2D::SetSpriteScale(float scale)
{
	Drawable2D::SetScale(glm::vec2(mTextureRespectiveScale.x * scale, mTextureRespectiveScale.y * scale));
}

} // namespace rendering_engine