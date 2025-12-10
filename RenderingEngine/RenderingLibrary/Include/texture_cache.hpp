// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_renderer_observer.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace rendering_engine
{
class IRenderer;
class ITextureRenderResources;
class ImageDataGpu;

/**
 * @class TextureCache
 * @brief Manages texture loading, GPU uploading, and caching for reuse.
 *
 * This class handles texture resource management at runtime.
 * It supports loading image files (JPG and PNG) from a folder or path into RAM, uploading them to GPU,
 * and provides access to the texture resources through string identifiers (filenames without extensions).
 *
 * Textures are stored using shared pointers to ImageDataGpu, and memory tracking is performed for both RAM and GPU sides.
 */
class TextureCache : public IRendererObserver
{
public:
    /**
     * @brief Constructs the TextureCache with a reference to the renderer.
     * @param renderer Pointer to the active rendering backend.
     */
    TextureCache(IRenderer* renderer);
    ~TextureCache();

    /**
     * @brief Loads all supported texture files (*.jpg, *.png) from a specified folder into RAM and uploads them to the GPU.
     *
     * Each texture is registered under its base filename (without extension) for later access.
     * Invalid paths or unsupported file types are skipped silently.
     *
     * @param pathToFolder Absolute or relative path to the folder containing texture files.
     */
    void LoadTexturesFromFolder(std::string pathToFolder);

    /**
     * @brief Loads all textures from the packed asset archive.
     *
     * This function behaves similarly to LoadTexturesFromFolder(), but instead of
     * scanning a filesystem directory, it iterates over virtual file entries
     * stored inside the packed content system created by the packaging tool.
     *
     * Only entries located under the virtual folder:
     *     "Textures/"
     * are considered valid texture resources.
     *
     * For each entry, the raw file bytes are retrieved via Utility::ReadPackedFile(),
     * decoded into an ImageDataGpu instance, and stored in the RAM cache.
     * GPU uploading must still be triggered via UploadTextureToGPU().
     */
    void LoadTexturesFromPackage();

    /**
     * @brief Loads a single texture into RAM from the given file path.
     *
     * The texture will not be uploaded to GPU automatically. The caller must use UploadTextureToGPU()
     * after this call if GPU usage is needed.
     *
     * @param path Full file path to the texture image (JPG/PNG).
     * @return The base filename used as the cache key, or an empty string if loading failed.
     */
    std::string UploadTextureToRAM(std::string path);

    /**
     * @brief Loads a single texture into RAM from raw file bytes.
     *
     * This overload enables texture loading from virtual files stored in a
     * packed asset archive or from any memory-based source rather than the OS
     * filesystem.
     *
     * The fileBytes buffer must contain the complete encoded texture data
     * (e.g., PNG or JPEG). The data is decoded and stored inside ImageDataGpu.
     *
     * @param textureFileName Cache key representing the virtual texture path
     *        (e.g., "Textures/myTexture.png" or simply "myTexture").
     * @param fileBytes Raw encoded texture file data (PNG/JPG).
     * @return The cache key on success, or an empty string on failure.
     */
    std::string UploadTextureToRAM(std::string textureFileName, std::vector<uint8_t> const& fileBytes);
    
    /**
     * @brief Uploads a texture (previously loaded into RAM) to GPU.
     *
     * If the texture is not found in RAM cache, the function does nothing.
     *
     * @param filename Base filename (no path, no extension) used as the cache key.
     */
    void UploadTextureToGPU(std::string filename);

    /**
     * @brief Releases a texture from GPU memory.
     *
     * The texture remains in RAM cache and can be uploaded again later.
     *
     * @param filename Cache key of the texture to release (base filename).
     */
    void ReleaseTextureFromGPU(std::string filename);

    /**
     * @brief Releases all cached textures from GPU memory.
     *
     * This does not affect RAM-side caching.
     */
    void ReleaseAllFromGPU();

    /**
     * @brief Fully clears the texture cache, including both RAM and GPU resources.
     */
    void ReleaseAll();

    /**
     * @brief Retrieves the full texture resource wrapper from cache.
     *
     * @param filename Base filename used during loading.
     * @return Shared pointer to ImageDataGpu, or nullptr if not found.
     */
    std::shared_ptr<ImageDataGpu> GetTextureResources(std::string filename);

    /**
     * @brief Retrieves only the GPU render resources of a cached texture.
     *
     * Useful when the caller only needs GPU handles and does not care about image metadata.
     *
     * @param filename Base filename used during loading.
     * @return Raw pointer to ITextureRenderResources, or nullptr if not found or not uploaded to GPU.
     */
    ITextureRenderResources* GetTextureRenderResources(std::string filename);

    /**
     * @brief Gets the total size of all currently cached textures in RAM.
     * @return Memory usage in bytes.
     */
    size_t GetSizeInRAM() const;

    /**
     * @brief Gets the total size of all textures currently uploaded to GPU.
     * @return GPU memory usage in bytes.
     */
    size_t GetSizeInGPU() const;

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
    std::unordered_map<std::string, std::shared_ptr<ImageDataGpu>> mTextures;

    size_t mTotalSizeRAM;
    size_t mTotalSizeGPU;
};

} // namespace rendering_engine
