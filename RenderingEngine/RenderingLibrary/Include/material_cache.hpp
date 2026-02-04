// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "i_renderer_observer.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace rendering_engine
{
class IRenderer;
class Material;
struct MaterialSettings;

/**
 * @class MaterialCache
 * @brief Manages creation, storage, and lifecycle of Material objects within the rendering engine.
 *
 * MaterialCache serves as a centralized system to instantiate, store, and retrieve materials used in the engine.
 * It supports both manually added materials and, in the future, loading materials from JSON definitions.
 * @see Jira EPIC: MAT-101 - Node-Based Material Editor
 *
 * It observes renderer events (such as swapchain recreation) to reinitialize or release GPU resources tied to materials.
 */
class MaterialCache : public IRendererObserver
{
public:
	/**
	 * @brief Constructs a MaterialCache and registers it as a renderer observer.
	 * @param renderer Pointer to the renderer interface used for creating GPU resources.
	 */
	MaterialCache(IRenderer* renderer);
	/**
	 * @brief Destructor. Automatically unregisters from the renderer.
	 */
	~MaterialCache();

	/**
	 * @brief Creates built-in materials for testing or demonstration.
	 *
	 * This is a temporary function to manually define materials in code.
	 * In the future, materials will be created by parsing JSON files from a materials folder.
	 */
	void CreateBuildInMaterials();
	
	/**
	 * @brief Adds a new Material instance using the provided settings.
	 * @param matSettings MaterialSettings struct containing metadata and configuration for the new material.
	 *
	 * If a material with the same name already exists, it will be replaced.
	 */
	void AddMaterial(MaterialSettings matSettings);

	/**
	 * @brief Retrieves a pointer to a Material instance by name.
	 * @param materialName Name of the material to retrieve.
	 * @return Pointer to the Material instance, or nullptr if not found.
	 */
	Material* GetMaterial(std::string materialName);

	/**
	 * @brief Removes a material from the cache.
	 * @param materialName Name of the material to remove.
	 */
	void RemoveMaterial(std::string materialName);

	/**
	 * @brief Releases all GPU resources associated with cached materials.
	 *
	 * Iterates through every material in the cache and calls their resource
	 * release routines, freeing descriptor set layouts, pipelines, and any other
	 * backend allocations. Unlike @ref RemoveMaterial, this does not erase the
	 * material records themselves from the cache, only their GPU-side resources.
	 *
	 * @details
	 * - Use when unloading a scene or before destroying the renderer to ensure
	 *   all GPU resources are released in a controlled manner.
	 * - After calling this, materials remain in the cache but are not renderable
	 *   until @ref OnRenderResourcesRebuild (or per-material reinitialization)
	 *   is performed.
	 * - Safe to call multiple times; redundant calls will have no effect once
	 *   resources are released.
	 *
	 * @see RemoveMaterial()
	 * @see OnRenderResourcesRelease()
	 * @see OnRenderResourcesRebuild()
	 */
	void ReleaseAll();

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
	std::unordered_map<std::string, std::shared_ptr<Material>> mMaterials;
};

} // namespace rendering_engine
