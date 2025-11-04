// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include "vertex_declarations.hpp"

namespace rendering_engine
{
class IRenderer;
class IMeshRenderResources;
class Model;

/**
 * @brief Types of supported mesh layouts for GPU upload and rendering.
 */
enum class MeshType
{
	None,      ///< Undefined or not yet set.
	Sprite2D,  ///< 2D sprite mesh (typically for UI or simple quads).
	Billboard, ///< 3D billboard mesh (sprites that face the camera).
	Surface    ///< Full 3D surface mesh (models with normals/tangents, etc.).
};

/**
 * @brief Manages mesh data in RAM and GPU, including upload and release operations.
 *
 * Supports on-demand composition of different vertex buffer formats and maintains
 * all intermediate mesh data required for rendering or further processing (e.g., collision).
 */
class MeshDataGpu
{
public:
	/**
	 * @brief Construct a MeshDataGpu instance.
	 * @param renderer Pointer to the rendering backend interface.
	 */
	MeshDataGpu(IRenderer* renderer);

	/**
	 * @brief Destructor. Releases GPU resources if allocated.
	 */
	~MeshDataGpu();

	/**
	 * @brief Upload mesh data to the GPU using the current mesh type.
	 */
	void UploadToGPU();

	/**
	 * @brief Release GPU resources associated with this mesh.
	 */
	void ReleaseFromGPU();

	/**
	 * @brief Check if mesh data is currently uploaded to GPU.
	 * @return True if GPU resources are resident, false otherwise.
	 */
	bool IsOnGPU() const;

	/**
	 * @brief Load a model from file and extract mesh data to RAM.
	 * @param path Path to the model file (e.g., FBX).
	 */
	void LoadModel(std::string path);

	/**
	 * @brief Creates a 1×1 unit quad centered at the origin.
	 *
	 * The quad spans from -0.5 to +0.5 on both X and Y axes and uses
	 * counterclockwise winding. Suitable as a base mesh for sprites
	 * or 2D UI elements.
	 */
	void CreateQuad2D();

	/**
	 * @brief Get the size (in bytes) of the vertex buffer on GPU.
	 * @return Size in bytes of the GPU vertex buffer.
	 */
	size_t GetGpuVertexBufferSize() const;

	/**
	 * @brief Get the size (in bytes) of the index buffer on GPU.
	 * @return Size in bytes of the GPU index buffer.
	 */
	size_t GetGpuIndexBufferSize()  const;

	/**
	 * @brief Get the size (in bytes) of the vertex buffer in RAM.
	 * @return Size in bytes of the CPU-side vertex data.
	 */
	size_t GetCpuVertexBufferSize() const;

	/**
	 * @brief Get the size (in bytes) of the index buffer in RAM.
	 * @return Size in bytes of the CPU-side index data.
	 */
	size_t GetCpuIndexBufferSize()  const;

	/**
	 * @brief Get the interface for mesh GPU resources (Vulkan or other backend).
	 * @return Pointer to the backend-specific IMeshRenderResources implementation.
	 */
	IMeshRenderResources* GetMeshRenderResources();

	/**
	 * @brief Returns a constant reference to the 2D vertex positions.
	 */
	const std::vector<glm::vec2>& GetPositions2D() const { return mPositions2D; }

	/**
	 * @brief Returns a constant reference to the 3D vertex positions.
	 */
	const std::vector<glm::vec3>& GetPositions() const { return mPositions; }

	/**
	 * @brief Returns a constant reference to the vertex colors.
	 */
	const std::vector<glm::vec4>& GetColors() const { return mColor; }

	/**
	 * @brief Returns a constant reference to the vertex normals.
	 */
	const std::vector<glm::vec3>& GetNormals() const { return mNormals; }

	/**
	 * @brief Returns a constant reference to the texture coordinates.
	 */
	const std::vector<glm::vec2>& GetTexCoords() const { return mTexCoords; }

	/**
	 * @brief Returns a constant reference to the vertex tangents.
	 */
	const std::vector<glm::vec3>& GetTangents() const { return mTangents; }

	/**
	 * @brief Returns a constant reference to the mesh indices.
	 */
	const std::vector<uint32_t>& GetIndices() const { return mIndices; }

protected:
	/**
	 * @brief Calculate and cache sizes of RAM buffers for statistics or debugging.
	 */
	void CalculateMeshParameters();

	/**
	 * @brief Compose a vertex buffer for 2D meshes (e.g., sprites).
	 * @return Vector of Vertex2D structures.
	 */
	std::vector<Vertex2D> ComposeVertex2DBuffer();

	/**
	 * @brief Compose a vertex buffer for unlit 3D meshes (e.g., billboards, particles).
	 * @return Vector of VertexPositionTextureColor structures.
	 */
	std::vector<VertexPositionColorTexture> ComposeUnlitBuffer();

	/**
	 * @brief Compose a vertex buffer for lit 3D meshes (with normals, tangents).
	 * @return Vector of VertexPositionTextureColorNormalTangent structures.
	 */
	std::vector<VertexPositionColorTextureNormalTangent> ComposeLitBuffer();

private:
	std::string mPath;
	IRenderer* mRenderer;

	MeshType mMeshType;

	std::vector<glm::vec2> mPositions2D;
	std::vector<glm::vec3> mPositions;
	std::vector<glm::vec4> mColor;
	std::vector<glm::vec3> mNormals;        
	std::vector<glm::vec2> mTexCoords;
	std::vector<glm::vec3> mTangents;       
	std::vector<uint32_t> mIndices;

	size_t mSizeOfVerticesBytes;
	size_t mSizeOfIndicesBytes;

	std::unique_ptr<IMeshRenderResources> mGpuHandle;
};

} // namespace rendering_engine