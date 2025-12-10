#include "model.hpp"
#include <stdexcept>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "model_material.hpp"

namespace rendering_engine
{
Model::Model(const std::string& filename, bool flipUVs)
{
    Assimp::Importer importer;

    uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
    if( flipUVs )
    {
        flags |= aiProcess_FlipUVs;
    }

    const aiScene* scene = importer.ReadFile(filename, flags);

    if( scene == nullptr )
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    if( scene->HasMaterials() )
    {
        for( uint32_t i = 0; i < scene->mNumMaterials; i++ )
        {
            mMaterials.push_back(std::shared_ptr<ModelMaterial>(new ModelMaterial(*this, *scene->mMaterials[i])));
        }
    }

    if( scene->HasMeshes() )
    {
        for( uint32_t i = 0; i < scene->mNumMeshes; i++ )
        {
            mMeshes.push_back(std::shared_ptr<Mesh>(new Mesh(*this, *(scene->mMeshes[i]))));
        }
    }
}
Model::Model(std::vector<uint8_t> const& fileBytes, bool flipUVs)
{
    Assimp::Importer importer;

    uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
    if (flipUVs)
    {
        flags |= aiProcess_FlipUVs;
    }

    const aiScene* scene = importer.ReadFileFromMemory(fileBytes.data(), fileBytes.size(), flags, "fbx");

    if (scene == nullptr)
    {
        throw std::runtime_error(importer.GetErrorString());
    }
    
    if (scene->HasMaterials())
    {
        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            mMaterials.push_back(std::shared_ptr<ModelMaterial>(new ModelMaterial(*this, *scene->mMaterials[i])));
        }
    }

    if (scene->HasMeshes())
    {
        for (uint32_t i = 0; i < scene->mNumMeshes; i++)
        {
            mMeshes.push_back(std::shared_ptr<Mesh>(new Mesh(*this, *(scene->mMeshes[i]))));
        }
    }
}
bool Model::HasMeshes() const
{
    return !mMeshes.empty();
}
bool Model::HasMaterials() const
{
    return !mMaterials.empty();
}
const std::vector<std::shared_ptr<Mesh>>& Model::Meshes() const
{
    return mMeshes;
}
const std::vector<std::shared_ptr<ModelMaterial>>& Model::Materials() const
{
    return mMaterials;
}
} //namespace rendering_engine