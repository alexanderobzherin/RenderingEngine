// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <string>
#include <memory>
#include <vector>

namespace rendering_engine
{
class IRenderer;
class ITextureRenderResources;
class ImageData;

/**
 * @class ImageDataGpu
 * @brief Combines CPU-side and GPU-side texture representations, managing both lifetimes and transitions.
 *
 * This class owns the image data in RAM (via ImageData) and optionally uploads/releases
 * the corresponding GPU-side representation. It provides accessors for both and tracks memory usage.
 */
class ImageDataGpu
{
public:
    /**
     * @brief Constructs an ImageDataGpu object from an image file path.
     * @param path Path to the image file (must be supported format such as .png or .jpg).
     * @param renderer Pointer to the rendering engine interface (IRenderer).
     */
    ImageDataGpu(std::string path, IRenderer* renderer);

    /**
     * @brief Constructs an ImageDataGpu object from raw image file bytes.
     *
     * This constructor allows loading GPU-ready image data directly from a memory
     * buffer rather than from disk. The input is interpreted as an encoded image
     * (PNG or JPEG). After decoding via ImageData, the pixel data is uploaded to
     * GPU memory through the provided IRenderer interface.
     *
     * @param fileBytes Raw binary contents of an image file (PNG or JPEG).
     * @param renderer Pointer to the rendering engine interface responsible for
     *                 uploading the texture to GPU memory.
     */
    ImageDataGpu(std::vector<uint8_t> const& fileBytes, IRenderer* renderer);

    ImageDataGpu(ImageData imageData, IRenderer* renderer);

    /**
     * @brief Destructor. Frees GPU memory if allocated.
     */
    ~ImageDataGpu();

    /**
     * @brief Uploads the texture data to GPU if not already uploaded.
     *
     * This function instantiates GPU resource handlers
     * and invokes texture uploading routines.
     */
    void UploadToGPU();

    /**
     * @brief Releases the GPU resources (image, sampler, view).
     *
     * After this call, the texture will be stored in RAM only.
     */
    void ReleaseFromGPU();

    /**
     * @brief Checks whether the texture is currently loaded in GPU memory.
     * @return true if texture has been uploaded to GPU, false otherwise.
     */
    bool IsOnGPU() const;

    /**
     * @brief Gets the size of the texture in RAM (in bytes).
     * @return Estimated RAM usage of the image data.
     */
    size_t GetSizeInRAM() const;

    /**
     * @brief Gets the size of the texture in GPU memory (in bytes).
     * @return Estimated GPU memory usage if uploaded, 0 otherwise.
     */
    size_t GetSizeInGPU() const;

    /**
     * @brief Returns a const reference to the CPU-side image data.
     * @return Const reference to the underlying ImageData.
     */
    ImageData const& GetCpuImageData() const;

    /**
     * @brief Returns a modifiable reference to the CPU-side image data.
     * @return Reference to the underlying ImageData.
     */
    ImageData& GetCpuImageData();

    /**
     * @brief Returns a pointer to the GPU texture resource interface.
     * @return Pointer to ITextureRenderResources if uploaded, nullptr otherwise.
     */
    ITextureRenderResources* GetTextureRenderResources() const;

private:
    const std::string mPath;
    const IRenderer* mRenderer;
    std::unique_ptr<ImageData> mImageData;
    std::unique_ptr<ITextureRenderResources> mGpuHandle;
};

} // namespace rendering_engine