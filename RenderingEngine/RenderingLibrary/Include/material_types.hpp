// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

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
 * @brief Describes the layout of a single packed parameter inside a uniform buffer.
 */
struct MaterialParameterLayoutEntry
{
    std::string name;
    size_t offset;
    size_t size;
    enum class Type { Float, Vec2, Vec3, Vec4 };
    Type type;
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

    const std::vector<MaterialParameterLayoutEntry>* parameterLayout = nullptr;
};

/**
 * @brief Contains the raw buffer data and layout metadata of packed material parameters.
 */
struct PackedMaterialData
{
    std::vector<uint8_t> buffer;
    std::vector<MaterialParameterLayoutEntry> layout;

	void SetMaterialFloat(const std::string& name, float value)
	{
		for (const auto& entry : layout)
		{
			if (entry.name == name &&
				entry.type == MaterialParameterLayoutEntry::Type::Float)
			{
				std::memcpy(
					buffer.data() + entry.offset,
					&value,
					sizeof(float)
				);
				return;
			}
		}
	}

	void SetMaterialVec2(const std::string& name, const glm::vec2& value)
	{
		for (const auto& entry : layout)
		{
			if (entry.name == name &&
				entry.type == MaterialParameterLayoutEntry::Type::Vec2)
			{
				std::memcpy(
					buffer.data() + entry.offset,
					&value,
					sizeof(glm::vec2)
				);
				return;
			}
		}
	}

	void SetMaterialVec3(const std::string& name, const glm::vec3& value)
	{
		for (const auto& entry : layout)
		{
			if (entry.name == name &&
				entry.type == MaterialParameterLayoutEntry::Type::Vec3)
			{
				std::memcpy(
					buffer.data() + entry.offset,
					&value,
					sizeof(glm::vec3)
				);
				return;
			}
		}
	}

	void SetMaterialVec4(const std::string& name, const glm::vec4& value)
	{
		for (const auto& entry : layout)
		{
			if (entry.name == name && entry.type == MaterialParameterLayoutEntry::Type::Vec4)
			{
				std::memcpy(
					buffer.data() + entry.offset,
					&value,
					sizeof(glm::vec4)
				);
				return;
			}
		}
	}

};

static const std::vector<MaterialParameterLayoutEntry> Font2DLayout =
{
    { "FontColor",   0, 16, MaterialParameterLayoutEntry::Type::Vec4 },
	{ "OutlineColor",       16, 16, MaterialParameterLayoutEntry::Type::Vec4 },
	{ "OutlineThicknessPx", 32, 4,  MaterialParameterLayoutEntry::Type::Float },
	{ "InvAtlasSizeWidth",       36, 4,  MaterialParameterLayoutEntry::Type::Float },
	{ "InvAtlasSizeHeight",       40, 4,  MaterialParameterLayoutEntry::Type::Float }
};

} // namespace rendering_engine
