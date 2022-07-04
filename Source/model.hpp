#pragma once

#include "mesh.hpp"

namespace rendering_engine
{
class ModelMaterial;

class Model final
{
friend class Mesh;

public:
    explicit Model(const std::string& filename, bool flipUVs = false);
    Model(const Model&) = default;
    Model& operator=(const Model&) = default;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;
    ~Model() = default;

    bool HasMeshes() const;
    bool HasMaterials() const;

    const std::vector<std::shared_ptr<Mesh>>& Meshes() const;
    const std::vector<std::shared_ptr<ModelMaterial>>& Materials() const;

private:
    std::vector<std::shared_ptr<Mesh>> mMeshes;
    std::vector<std::shared_ptr<ModelMaterial>> mMaterials;
};
}// namespace rendering_engine