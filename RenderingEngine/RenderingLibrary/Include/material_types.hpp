// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <vector>
#include <cstdint>

namespace rendering_engine
{

/**
 * @brief Specifies the rendering domain where the material is used.
 */
enum class MaterialDomain
{
    Surface3D,
    Sprite2D
};

/**
 * @brief Specifies how the material handles transparency.
 */
enum class BlendMode
{
    Opaque,
    Translucent
};

/**
 * @brief Specifies the shading model used by the material.
 */
enum class ShadingModel
{
    Lit,
    Unlit
};

/**
 * @brief Settings required to define a material instance.
 */
struct MaterialSettings
{
    std::string parentMaterialName; // to reuse same shader code
    std::string materialName;
    MaterialDomain materialDomain;
    BlendMode blendMode;
    ShadingModel shadingModel;
};

/**
 * @brief Describes the layout of a single packed parameter inside a uniform buffer.
 */
struct MaterialParameterLayoutEntry
{
    std::string name;
    size_t offset;
    size_t size;
    enum class Type { Float, Vec3, Vec4 };
    Type type;
};

/**
 * @brief Contains the raw buffer data and layout metadata of packed material parameters.
 */
struct PackedMaterialData
{
    std::vector<uint8_t> buffer;
    std::vector<MaterialParameterLayoutEntry> layout;
};

} // namespace rendering_engine
