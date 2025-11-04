// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
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
class Model;
/**
 * @class Mesh
 * @brief Represents a single geometric mesh imported from a model file.
 *
 * The Mesh class stores all CPU-side vertex attributes and index data for one
 * mesh, as imported from an Assimp `aiMesh`. It acts as an intermediate
 * container before the data is transferred to GPU buffers (via MeshDataGpu).
 *
 * Each Mesh instance is associated with:
 * - One parent `Model`
 * - One `ModelMaterial` describing surface properties
 * - One or more vertex attribute streams (positions, normals, tangents, etc.)
 *
 * @note This class does not handle GPU resource management.
 * @see Model, ModelMaterial, MeshDataGpu
 */
class Mesh
{
	friend class Model;

public:
	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;
	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;
	~Mesh() = default;
    /**
     * @brief Returns the parent model that owns this mesh.
     * @return Reference to the parent Model.
     */
    Model& GetModel();
    /**
     * @brief Returns the material associated with this mesh.
     * @return Shared pointer to ModelMaterial.
     */
    std::shared_ptr<ModelMaterial> GetMaterial() const;
    /**
     * @brief Returns the mesh name as imported from the source file.
     * @return Mesh name string.
     */
    const std::string& Name() const;
    /**
     * @brief Returns the vertex position array.
     * @return Vector of vertex positions (glm::vec3).
     */
    const std::vector<glm::vec3>& Vertices() const;
    /**
     * @brief Returns the vertex normal array.
     * @return Vector of normal vectors (glm::vec3).
     */
    const std::vector<glm::vec3>& Normals() const;
    /**
     * @brief Returns the vertex tangent array.
     * @return Vector of tangent vectors (glm::vec3).
     */
    const std::vector<glm::vec3>& Tangents() const;
    /**
     * @brief Returns the vertex binormal array.
     * @return Vector of binormal vectors (glm::vec3).
     */
    const std::vector<glm::vec3>& BiNormals() const;
    /**
     * @brief Returns texture coordinate sets.
     *
     * Each inner vector represents one texture coordinate channel.
     * Assimp supports up to 8 UV sets per mesh.
     *
     * @return Vector of texture coordinate arrays.
     */
    const std::vector<std::vector<glm::vec3>>& TextureCoordinates() const;
    /**
     * @brief Returns per-vertex color sets.
     *
     * Each inner vector corresponds to one color channel.
     *
     * @return Vector of vertex color arrays (each color is a vec4).
     */
    const std::vector<std::vector<glm::vec4>>& VertexColors() const;
    /**
     * @brief Returns the number of faces in the mesh.
     * @return Face count.
     */
    std::uint32_t FaceCount() const;
    /**
     * @brief Returns the vertex index buffer.
     * @return Vector of vertex indices.
     */
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

} // namespace rendering_engine