#include "material_cache.hpp"
#include "i_renderer.hpp"
#include "material.hpp"
#include "i_material_render_resources.hpp"

namespace rendering_engine
{

MaterialCache::MaterialCache(IRenderer* renderer)
	:
	mRenderer(renderer)
{
	mRenderer->RegisterObserver(this);
}

MaterialCache::~MaterialCache()
{
	mRenderer->UnregisterObserver(this);
}

void MaterialCache::CreateBuildInMaterials()
{
	// BasicTexture3D material
	MaterialSettings setBasicTexture3D;
	setBasicTexture3D.materialName = std::string{ "BasicTexture3D" };
	setBasicTexture3D.materialDomain = MaterialDomain::Surface3D;
	setBasicTexture3D.shadingModel = ShadingModel::Unlit;
	setBasicTexture3D.blendMode = BlendMode::Opaque;

	AddMaterial(setBasicTexture3D);
	Material* basicTexture3DMat = GetMaterial(setBasicTexture3D.materialName);
	if (basicTexture3DMat)
	{
		basicTexture3DMat->AddTexture("test_cube_color");
		//basicTexture3DMat->AddTexture("PNG_transparency_demonstration_1");
	}
	basicTexture3DMat->InitializeRenderResources();

	// FlatColorFiltering material
	MaterialSettings setFlatColorFiltering;
	setFlatColorFiltering.materialName = std::string{ "FlatColorFiltering" };
	setFlatColorFiltering.materialDomain = MaterialDomain::Surface3D;
	setFlatColorFiltering.shadingModel = ShadingModel::Unlit;
	setFlatColorFiltering.blendMode = BlendMode::Opaque;
	
	AddMaterial(setFlatColorFiltering);
	Material* flatColorFilteringMat = GetMaterial(setFlatColorFiltering.materialName);
	if (flatColorFilteringMat)
	{
		flatColorFilteringMat->AddTexture("D4FontTextureSegoeScript");
	}
	flatColorFilteringMat->InitializeRenderResources();

	// Test sprite 2d
	MaterialSettings setTestSprite2d;
	setTestSprite2d.materialName = std::string{ "Quad2D" };
	setTestSprite2d.materialDomain = MaterialDomain::Sprite2D;
	setTestSprite2d.shadingModel = ShadingModel::Unlit;
	setTestSprite2d.blendMode = BlendMode::Opaque;

	AddMaterial(setTestSprite2d);
	Material* testSprite2dMat = GetMaterial(setTestSprite2d.materialName);
	if (testSprite2dMat)
	{
		testSprite2dMat->AddTexture("PNG_transparency_demonstration_1");
	}

	testSprite2dMat->InitializeRenderResources();
}

void MaterialCache::AddMaterial(MaterialSettings matSettings)
{
	if (matSettings.materialName.empty())
	{
		// Log incorrect material name
		return;
	}
	mMaterials[matSettings.materialName] = std::make_shared<Material>(mRenderer, matSettings);
}

Material* MaterialCache::GetMaterial(std::string materialName)
{
	if (auto search = mMaterials.find(materialName); search == mMaterials.end())
	{
		return nullptr;
	}

	return mMaterials.at(materialName).get();
}

void MaterialCache::RemoveMaterial(std::string materialName)
{
	if (auto it = mMaterials.find(materialName); it != mMaterials.end())
	{
		it->second->ReleaseRenderResources();
		mMaterials.erase(it);
	}
}

void MaterialCache::ReleaseAll()
{
	OnRenderResourcesRelease();
}

void MaterialCache::OnRenderResourcesRelease()
{
	for (auto& material : mMaterials)
	{
		material.second->ReleaseRenderResources();
	}
}

void MaterialCache::OnRenderResourcesRebuild()
{
	for (auto& material : mMaterials)
	{
		material.second->InitializeRenderResources();
	}
}

}