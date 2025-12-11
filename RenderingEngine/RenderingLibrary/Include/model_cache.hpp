// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_renderer_observer.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace rendering_engine
{
class IRenderer;
class IMeshRenderResources;
class MeshDataGpu;

/**
 * @brief Manages loading, caching, and GPU residency of all model/mesh resources.
 *
 * Responsible for importing model files into RAM, uploading mesh data to GPU memory,
 * and managing the lifetime and statistics of all managed models in the rendering engine.
 * Also observes renderer resource lifecycle events (release/rebuild).
 */
class ModelCache : public IRendererObserver
{
public:
	/**
	 * @brief Construct a ModelCache and register it with the given renderer.
	 * @param renderer Pointer to the owning IRenderer implementation.
	 */
	ModelCache(IRenderer* renderer);

	/**
	 * @brief Destructor. Releases all resources and unregisters observer.
	 */
	~ModelCache();

	/**
	 * @brief Load all models found in the specified folder into RAM.
	 * @param pathToFolder Path to the directory containing model files.
	 */
	void LoadModelsFromFolder(std::string pathToFolder);

	/**
     * @brief Load all models from the packed asset container.
     *
     * This function behaves similarly to LoadModelsFromFolder(), but instead
     * retrieves model files from the packed asset system created by the
     * Packaging Tool. Each packed file is read into memory and processed via
     * UploadModelToRAM(std::string, const std::vector<uint8_t>&).
     *
     * Only file entries located under the virtual folder:
     *     "Models/"
     * are considered.
     *
     * The resulting MeshDataGpu objects remain cached and ready for GPU upload.
     */
	void LoadModelsFromPackage();

	/**
	 * @brief Creates a built-in 2D quad mesh.
	 *
	 * This helper is used by 2D systems (e.g., UI, sprites) to create a reusable
	 * quad mesh without external model files.
	 */
	void CreateQuad2D();

	/**
	 * @brief Load a single model from file into RAM.
	 * @param path Path to the model file.
	 * @return The filename used as a cache key, or empty string on failure.
	 */
	std::string UploadModelToRAM(std::string path);

	/**
     * @brief Load a single model into RAM from a raw file buffer.
     *
     * This overload is used when the model originates from a packed asset
     * archive or any virtual filesystem source rather than the OS filesystem.
     *
     * @param fileName Cache key (typically relative virtual path, e.g. "Models/cube.fbx").
     * @param fileBytes Raw contents of the model file.
     * @return The cache key on success, or an empty string on failure.
     */
	std::string UploadModelToRAM(std::string fileName, std::vector<uint8_t> const& fileBytes);

	/**
	 * @brief Upload a cached model's mesh data to the GPU.
	 * @param filename The model's cache key (filename without extension).
	 */
	void UploadModelToGPU(std::string filename);

	/**
	 * @brief Release a model's mesh data from GPU memory.
	 * @param filename The model's cache key (filename without extension).
	 */
	void ReleaseModelFromGPU(std::string filename);

	/**
	 * @brief Release all model mesh data from GPU memory.
	 */
	void ReleaseAllFromGPU();

	/**
	 * @brief Remove all models from both GPU and RAM, clearing the cache.
	 */
	void ReleaseAll();

	/**
	 * @brief Get a shared pointer to the MeshDataGpu for a model.
	 * @param filename The model's cache key (filename without extension).
	 * @return Shared pointer to MeshDataGpu, or nullptr if not found.
	 */
	std::shared_ptr<MeshDataGpu> GetMeshResources(std::string filename);

	/**
	 * @brief Get the IMeshRenderResources interface for a model's GPU resources.
	 * @param filename The model's cache key (filename without extension).
	 * @return Pointer to IMeshRenderResources, or nullptr if not found or not on GPU.
	 */
	IMeshRenderResources* GetMeshRenderResources(std::string filename);

	/**
	 * @brief Get the total size (in bytes) of all models loaded in RAM.
	 * @return Total RAM usage in bytes.
	 */
	inline size_t GetSizeInRAM() const;

	/**
	 * @brief Get the total size (in bytes) of all models currently resident on GPU.
	 * @return Total GPU usage in bytes.
	 */
	inline size_t GetSizeInGPU() const;

protected:
	/**
	 * @copydoc IRendererObserver::OnRenderResourcesRelease
	 */
	void OnRenderResourcesRelease() override;
	/**
	 * @copydoc IRendererObserver::OnRenderResourcesRebuild
	 */
	void OnRenderResourcesRebuild() override;

protected:
	IRenderer* mRenderer;
	std::unordered_map<std::string, std::shared_ptr<MeshDataGpu>> mModels;

	size_t mTotalSizeRAM;
	size_t mTotalSizeGPU;
};



} // namespace rendering_engine