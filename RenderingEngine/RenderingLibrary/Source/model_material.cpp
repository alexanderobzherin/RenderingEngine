#include "model_material.hpp"
#include "assimp/scene.h"

namespace rendering_engine
{
const std::map<TextureType, uint32_t> ModelMaterial::sTextureTypeMappings =
{
	{ TextureType::Difffuse, aiTextureType_DIFFUSE },
	{ TextureType::SpecularMap, aiTextureType_SPECULAR },
	{ TextureType::Ambient, aiTextureType_AMBIENT },
	{ TextureType::Emissive, aiTextureType_EMISSIVE },
	{ TextureType::Difffuse, aiTextureType_DIFFUSE },
	{ TextureType::Heightmap, aiTextureType_HEIGHT },
	{ TextureType::NormalMap, aiTextureType_NORMALS },
	{ TextureType::SpecularPowerMap, aiTextureType_SHININESS },
	{ TextureType::DisplacementMap, aiTextureType_DISPLACEMENT },
	{ TextureType::LightMap, aiTextureType_LIGHTMAP },
};

ModelMaterial::ModelMaterial(Model& model)
	:
	mModel(&model)
{
}

Model& ModelMaterial::GetModel()
{
	return *mModel;
}

const std::string& ModelMaterial::Name() const
{
	return mName;
}

const std::map<TextureType, std::vector<std::string>> ModelMaterial::Textures() const
{
	return mTextures;
}

ModelMaterial::ModelMaterial(Model& model, aiMaterial& material)
	:
	mModel(&model)
{
	aiString name;
	material.Get(AI_MATKEY_NAME, name);
	mName = name.C_Str();

	for( TextureType textureType : Enum<TextureType>() )
	{
		aiTextureType mappedTextureType = static_cast<aiTextureType>(sTextureTypeMappings.at(textureType));

		uint32_t textureCount = material.GetTextureCount(mappedTextureType);
		if( textureCount > 0 )
		{
			std::vector<std::string> textures;
			textures.reserve(textureCount);
			for( uint32_t textureIndex = 0; textureIndex < textureCount; textureIndex++ )
			{
				aiString path;
				if( material.GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS )
				{
					textures.push_back(path.C_Str());
				}
			}
			mTextures.emplace(textureType, move(textures));
		}
	}
}

}// namespace rendering_engine