// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

namespace rendering_engine
{
class ImageData;

/**
 * @class ITextureRenderResources
 * @brief Interface for backend-specific GPU texture resource management.
 *
 * This interface defines the core functions required to manage the lifecycle
 * of texture data uploaded to the GPU. It enables backends like Vulkan, OpenGL,
 * or DirectX to implement their specific logic while staying abstracted from higher-level code.
 */
class ITextureRenderResources
{
public:
	/**
	 * @brief Virtual destructor.
	 */
	virtual ~ITextureRenderResources() = default;

	/**
	 * @brief Uploads texture data from CPU to GPU memory.
	 *
	 * @param data Reference to the CPU-resident image data.
	 */
	virtual void LoadToGPU(const ImageData& data) = 0;

	/**
	 * @brief Releases texture memory from the GPU.
	 */
	virtual void ReleaseFromGPU() = 0;

	/**
	 * @brief Checks whether the texture is currently loaded in GPU memory.
	 *
	 * @return true if the texture is loaded, false otherwise.
	 */
	virtual bool IsTextureLoadedInGPU() = 0;

	/**
	 * @brief Returns the memory footprint of the texture on the GPU.
	 *
	 * @return Size of the texture data in GPU memory in bytes.
	 */
	virtual size_t GetSizeInGPUBytes() const = 0;
};
}