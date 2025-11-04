// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.
#pragma once

#include "mesh.hpp"

namespace rendering_engine
{
class ModelMaterial;

/**
 * @class Model
 * @brief Represents a 3D model composed of multiple meshes and materials.
 *
 * The Model class serves as an intermediate container between the model-loading
 * backend (e.g., Assimp) and GPU-side mesh upload (via MeshDataGpu). It holds
 * all meshes and materials associated with a 3D model file, typically loaded
 * from formats such as FBX, OBJ, or glTF.
 *
 * Each imported model can contain multiple meshes and materials, all of which
 * are accessible through this class for further processing or rendering.
 *
 * @note This class does not perform rendering. It only stores CPU-side mesh
 * and material data for later GPU upload.
 * @see Mesh, ModelMaterial, MeshDataGpu
 */
class Model final
{
friend class Mesh;

public:
    /**
     * @brief Constructs a model from a file.
     *
     * The model loader uses Assimp internally to parse the file and build
     * Mesh and ModelMaterial instances. Optionally, UV coordinates can be
     * flipped vertically during import.
     *
     * @param filename Path to the model file (e.g., `.obj`, `.fbx`, `.gltf`).
     * @param flipUVs Whether to flip UV coordinates vertically (useful for texture alignment).
     */
    explicit Model(const std::string& filename, bool flipUVs = false);
    Model(const Model&) = default;
    Model& operator=(const Model&) = default;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;
    ~Model() = default;

    /**
     * @brief Checks whether this model contains any meshes.
     * @return True if at least one mesh was loaded; false otherwise.
     */
    bool HasMeshes() const;
    /**
     * @brief Checks whether this model contains any materials.
     * @return True if at least one material was loaded; false otherwise.
     */
    bool HasMaterials() const;
    /**
     * @brief Returns the list of meshes belonging to this model.
     * @return Vector of shared pointers to Mesh objects.
     */
    const std::vector<std::shared_ptr<Mesh>>& Meshes() const;
    /**
     * @brief Returns the list of materials belonging to this model.
     * @return Vector of shared pointers to ModelMaterial objects.
     */
    const std::vector<std::shared_ptr<ModelMaterial>>& Materials() const;

private:
    std::vector<std::shared_ptr<Mesh>> mMeshes;
    std::vector<std::shared_ptr<ModelMaterial>> mMaterials;
};
}// namespace rendering_engine