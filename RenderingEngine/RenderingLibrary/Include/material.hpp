// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "material_types.hpp"

namespace rendering_engine
{
class IRenderer;
class IMaterialRenderResources;

/**
 * @brief Represents a material instance with parameter values, texture bindings, and rendering configuration.
 *
 * This class encapsulates both the CPU-side representation of a material (its parameters and textures),
 * and a pointer to its backend-specific GPU resource handle.
 */
class Material
{
public:
    /**
     * @brief Constructs a new Material instance.
     *
     * @param renderer Pointer to the rendering engine interface.
     * @param matSettings Settings that define the material behavior.
     */
    explicit Material(IRenderer* renderer, 
                      MaterialSettings matSettings);

    ~Material();
    /**
     * @brief Returns the material's static settings (domain, blend mode, shading model, etc.).
     */
    const MaterialSettings GetMaterialSettings() const;

    /**
     * @brief Initializes backend-specific GPU resources associated with this material.
     *
     * Must be called after construction and before rendering.
     */
    void InitializeRenderResources();
    /**
      * @brief Releases any GPU resources associated with this material.
      */
    void ReleaseRenderResources();

    /**
     * @brief Packs the current float/vector parameters into a binary buffer and layout metadata.
     *
     * This buffer can be uploaded to the GPU as a uniform buffer or push constants.
     *
     * @return PackedMaterialData Struct containing binary data and parameter layout info.
     */
    PackedMaterialData PackMaterialParameters();

    /**
     * @brief Returns the list of texture names used by this material.
     */
    std::vector<std::string> GetTextures() const;

    /**
     * @brief Returns the backend-specific GPU handle of the material.
     */
    IMaterialRenderResources* GetMaterialRenderResources() const;

    /**
     * @brief Sets a float parameter for the material.
     *
     * @param name Name of the parameter.
     * @param value Float value to set.
     */
    void SetFloat(const std::string& name, float value);

    /**
     * @brief Sets a vec3 parameter for the material.
     *
     * @param name Name of the parameter.
     * @param value glm::vec3 value to set.
     */
    void SetVec2(const std::string& name, glm::vec2 value);

    /**
     * @brief Sets a vec3 parameter for the material.
     *
     * @param name Name of the parameter.
     * @param value glm::vec3 value to set.
     */
    void SetVec3(const std::string& name, glm::vec3 value);

    /**
     * @brief Sets a vec4 parameter for the material.
     *
     * @param name Name of the parameter.
     * @param value glm::vec4 value to set.
     */
    void SetVec4(const std::string& name, glm::vec4 value);

    /**
     * @brief Adds a texture name to the material's list of used textures.
     *
     * @param textureName Name of the texture (e.g., texture ID or resource name).
     */
    void AddTexture(const std::string& textureName);

private:
    size_t std140_align(MaterialParameterLayoutEntry::Type type);
    size_t std140_size(MaterialParameterLayoutEntry::Type type);

private:
    std::unordered_map<std::string, float> mFloatParameters;
    std::unordered_map<std::string, glm::vec2> mVec2Parameters;
    std::unordered_map<std::string, glm::vec3> mVec3Parameters;
    std::unordered_map<std::string, glm::vec4> mVec4Parameters;

    MaterialSettings mMaterialSettings;
    std::vector<std::string> mTextures;

    IRenderer* mRenderer;
    std::unique_ptr<IMaterialRenderResources> mGpuHandle;
    const std::vector<MaterialParameterLayoutEntry>* mParameterLayout;
};

} // namespace rendering_engine
