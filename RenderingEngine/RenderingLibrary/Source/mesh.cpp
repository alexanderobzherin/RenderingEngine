#include "mesh.hpp"
#include "assimp/scene.h"
namespace rendering_engine
{
Mesh::Mesh(Model& model, aiMesh& mesh)
	:
	mModel(&model),
	mMaterial(nullptr),
	mName(mesh.mName.C_Str()),
	mVertices(),
	mNormals(),
	mTangents(),
	mBiNormals(),
	mTextureCoordinates(),
	mVertexColors(),
	mFaceCount(0),
	mIndices()
{
	mMaterial; // = mModel.Materials().at(mesh.mMaterialIndex);

       // Vertices
    mVertices.reserve(mesh.mNumVertices);
    for( uint32_t i = 0; i < mesh.mNumVertices; i++ )
    {
        const auto& vertex = mesh.mVertices[i];
        mVertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    // Normals
    if( mesh.HasNormals() )
    {
        mNormals.reserve(mesh.mNumVertices);
        for( uint32_t i = 0; i < mesh.mNumVertices; i++ )
        {
            const auto& normal = mesh.mNormals[i];
            mNormals.emplace_back(normal.x, normal.y, normal.z);
        }
    }

    // Tangents and Binormals
    if( mesh.HasTangentsAndBitangents() )
    {
        mTangents.reserve(mesh.mNumVertices);
        mBiNormals.reserve(mesh.mNumVertices);
        for( uint32_t i = 0; i < mesh.mNumVertices; i++ )
        {
            const auto& tangent = mesh.mTangents[i];
            mTangents.emplace_back(tangent.x, tangent.y, tangent.z);

            const auto& binormal = mesh.mBitangents[i];
            mBiNormals.emplace_back(binormal.x, binormal.y, binormal.z);
        }
    }

    // Texture Coordinates
    uint32_t uvChannelCount = mesh.GetNumUVChannels();
    for( uint32_t i = 0; i < uvChannelCount; i++ )
    {
        std::vector<glm::vec3> textureCoordinates;
        textureCoordinates.reserve(mesh.mNumVertices);
        aiVector3D* aiTextureCoordinates = mesh.mTextureCoords[i];
        for( uint32_t j = 0; j < mesh.mNumVertices; j++ )
        {
            const auto& uv = aiTextureCoordinates[j];
            textureCoordinates.emplace_back(uv.x, uv.y, uv.z);
        }

        mTextureCoordinates.push_back(std::move(textureCoordinates));
    }

    // Vertex Colors
    uint32_t colorChannelCount = mesh.GetNumColorChannels();
    for( uint32_t i = 0; i < colorChannelCount; i++ )
    {
        std::vector<glm::vec4> vertexColors;
        vertexColors.reserve(mesh.mNumVertices);
        aiColor4D* aiVertexColors = mesh.mColors[i];
        for( uint32_t j = 0; j < mesh.mNumVertices; j++ )
        {
            const auto& c = aiVertexColors[j];
            vertexColors.emplace_back(c.r, c.g, c.b, c.a);
        }
        mVertexColors.push_back(move(vertexColors));
    }

    // Faces (note: could pre-reserve if we limit primitive types)
    if( mesh.HasFaces() )
    {
        mFaceCount = mesh.mNumFaces;
        for( uint32_t i = 0; i < mFaceCount; i++ )
        {
            aiFace* face = &mesh.mFaces[i];

            for( uint32_t j = 0; j < face->mNumIndices; j++ )
            {
                mIndices.push_back(face->mIndices[j]);
            }
        }
    }
}


Model& Mesh::GetModel()
{
    return *mModel;
}

std::shared_ptr<ModelMaterial> Mesh::GetMaterial() const
{
    return mMaterial;
}

const std::string& Mesh::Name() const
{
    return mName;
}

const std::vector<glm::vec3>& Mesh::Vertices() const
{
    return mVertices;
}

const std::vector<glm::vec3>& Mesh::Normals() const
{
    return mNormals;
}

const std::vector<glm::vec3>& Mesh::Tangents() const
{
    return mTangents;
}

const std::vector<glm::vec3>& Mesh::BiNormals() const
{
    return mBiNormals;
}

const std::vector<std::vector<glm::vec3>>& Mesh::TextureCoordinates() const
{
    return mTextureCoordinates;
}

const std::vector<std::vector<glm::vec4>>& Mesh::VertexColors() const
{
    return mVertexColors;
}

std::uint32_t Mesh::FaceCount() const
{
    return mFaceCount;
}

const std::vector<std::uint32_t>& Mesh::Indices() const
{
    return mIndices;
}

}// namespace rendering_engine