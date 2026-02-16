// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

struct aiMaterial;

namespace rendering_engine
{
/**
 * @enum TextureType
 * @brief Enumerates supported texture map types for imported materials.
 *
 * These correspond to common texture slots used in 3D rendering pipelines
 * and align with Assimp's material texture types. The range [Begin, End]
 * allows easy iteration through all supported types.
 */
enum TextureType
{
	Difffuse = 0,
	SpecularMap,
	Ambient,
	Emissive,
	Heightmap,
	NormalMap,
	SpecularPowerMap,
	DisplacementMap,
	LightMap,

	Begin = Difffuse,
	End = LightMap
};

class Model;
/**
 * @class ModelMaterial
 * @brief Represents a single material imported from a 3D model file.
 *
 * A `ModelMaterial` acts as a lightweight wrapper around Assimp's `aiMaterial`,
 * extracting and storing references to texture file paths per texture type.
 *
 * @note Multiple meshes within a model may reference the same `ModelMaterial`.
 * @see Model, TextureType
 */
class ModelMaterial
{
	friend class Model;

public:
	/**
	 * @brief Constructs an empty material associated with a model.
	 * @param model Reference to the owning Model instance.
	 */
	explicit ModelMaterial(Model& model);

	ModelMaterial(const ModelMaterial&) = default;
	ModelMaterial& operator=(const ModelMaterial&) = default;
	ModelMaterial(ModelMaterial&&) = default;
	ModelMaterial& operator=(ModelMaterial&&) = default;
	~ModelMaterial() = default;
	/**
	 * @brief Returns a reference to the owning Model.
	 * @return Reference to the parent Model instance.
	 */
	Model& GetModel();
	/**
	 * @brief Gets the material name as defined in the source file.
	 * @return Material name string.
	 */
	const std::string& Name() const;
	/**
	 * @brief Retrieves the list of textures associated with each texture type.
	 *
	 * Each entry in the map corresponds to one TextureType key (e.g., Diffuse, NormalMap)
	 * and a list of texture file paths associated with that type.
	 *
	 * @return A map of texture types to vectors of texture file paths.
	 */
	const std::map<TextureType, std::vector<std::string>> Textures() const;

private:
	static const std::map<TextureType, std::uint32_t> sTextureTypeMappings;

	ModelMaterial(Model& model, aiMaterial& material);

	Model* mModel;
	std::string mName;
	std::map<TextureType, std::vector<std::string>> mTextures;
};

template <typename T>
class Enum final
{
public:
	class Iterator final
	{
	public:
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using reference = T;
		using pointer = T*;
		using iterator_category = std::forward_iterator_tag;

		Iterator() = default;
		explicit Iterator(T value);
		Iterator(const Iterator&) = default;
		Iterator(Iterator&&) = default;
		Iterator& operator=(const Iterator&) = default;
		Iterator& operator=(Iterator&&) = default;
		~Iterator() = default;

		T operator*() const;
		Iterator& operator++();
		Iterator operator++(int);

		bool operator==(const Iterator& rhs) const;
		bool operator!=(const Iterator& rhs) const;

	private:
		T mValue;
	};

	Iterator begin() const;
	Iterator end() const;
};

template <typename T>
inline Enum<T>::Iterator::Iterator(T value) :
	mValue(value)
{
}

template <typename T>
inline bool Enum<T>::Iterator::operator==(const Iterator& rhs) const
{
	return mValue == rhs.mValue;
}

template <typename T>
inline bool Enum<T>::Iterator::operator!=(const Iterator& rhs) const
{
	return mValue != rhs.mValue;
}

template <typename T>
inline T Enum<T>::Iterator::operator*() const
{
	if (mValue > T::End)
	{
		throw std::runtime_error("Cannot dereference end().");
	}

	return mValue;
}

template <typename T>
inline typename Enum<T>::Iterator& Enum<T>::Iterator::operator++()
{
	if (mValue <= T::End)
	{
		mValue = T(static_cast<int>(mValue) + 1);
	}
	return *this;
}

template <typename T>
inline typename Enum<T>::Iterator Enum<T>::Iterator::operator++(int)
{
	Iterator temp = *this;
	operator++();
	return temp;
}

template <typename T>
inline typename Enum<T>::Iterator Enum<T>::begin() const
{
	return Iterator(T::Begin);
}

template <typename T>
inline typename Enum<T>::Iterator Enum<T>::end() const
{
	return Iterator(T(static_cast<int>(T::End) + 1));
}

}// namespace rendering_engine