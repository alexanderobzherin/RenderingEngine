#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

struct aiMesh;

namespace rendering_engine
{
class ModelMaterial;

class Mesh
{
	friend class Model;

public:
	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;
	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;
	~Mesh() = default;

    Model& GetModel();
    std::shared_ptr<ModelMaterial> GetMaterial() const;
    const std::string& Name() const;

    const std::vector<glm::vec3>& Vertices() const;
    const std::vector<glm::vec3>& Normals() const;
    const std::vector<glm::vec3>& Tangents() const;
    const std::vector<glm::vec3>& BiNormals() const;
    const std::vector<std::vector<glm::vec3>>& TextureCoordinates() const;
    const std::vector<std::vector<glm::vec4>>& VertexColors() const;
    std::uint32_t FaceCount() const;
    const std::vector<std::uint32_t>& Indices() const;

private:
    Mesh(Model& model, aiMesh& mesh);

private:
    Model* mModel;
    std::shared_ptr<ModelMaterial> mMaterial;
    std::string mName;
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mNormals;
    std::vector<glm::vec3> mTangents;
    std::vector<glm::vec3> mBiNormals;
    std::vector<std::vector<glm::vec3>> mTextureCoordinates;
    std::vector<std::vector<glm::vec4>> mVertexColors;
    std::uint32_t mFaceCount;
    std::vector<std::uint32_t> mIndices;
};

}