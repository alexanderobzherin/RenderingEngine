#include "material.hpp"
#include "i_renderer.hpp"
#include "i_material_render_resources.hpp"

#include <cstring>

namespace rendering_engine
{

Material::Material(IRenderer* renderer, MaterialSettings matSettings)
    :
    mRenderer(renderer),
    mMaterialSettings(matSettings),
    mGpuHandle(nullptr)
{}
const MaterialSettings Material::GetMaterialSettings() const
{
    return mMaterialSettings;
}
void Material::InitializeRenderResources()
{
    mGpuHandle = std::unique_ptr<IMaterialRenderResources>(mRenderer->ProvideMaterialRenderResources());
    mGpuHandle->Initialize(this);
}
void Material::ReleaseRenderResources()
{
    mGpuHandle->Shutdown();
    mGpuHandle.release();
    mGpuHandle = nullptr;
}
PackedMaterialData Material::PackMaterialParameters()
{
    PackedMaterialData result;
    size_t currentOffset = 0;

    // floats
    for (const auto& [name, value] : mFloatParameters) 
    {
        size_t alignment = std140_align(MaterialParameterLayoutEntry::Type::Float);
        currentOffset = (currentOffset + alignment - 1) & ~(alignment - 1);
        result.layout.push_back({ name, currentOffset, 4, MaterialParameterLayoutEntry::Type::Float });
        result.buffer.resize(currentOffset + 4);
        std::memcpy(result.buffer.data() + currentOffset, &value, 4);
        currentOffset += 4;
    }
    // vec3
    for (const auto& [name, value] : mVec3Parameters) 
    {
        size_t alignment = std140_align(MaterialParameterLayoutEntry::Type::Vec3);
        currentOffset = (currentOffset + alignment - 1) & ~(alignment - 1);
        result.layout.push_back({ name, currentOffset, 12, MaterialParameterLayoutEntry::Type::Vec3 });
        result.buffer.resize(currentOffset + 16); // 16 bytes for std140 vec3
        std::memcpy(result.buffer.data() + currentOffset, &value, 12);
        // pad last 4 bytes with zero
        std::memset(result.buffer.data() + currentOffset + 12, 0, 4);
        currentOffset += 16;
    }
    // vec4
    for (const auto& [name, value] : mVec4Parameters) 
    {
        size_t alignment = std140_align(MaterialParameterLayoutEntry::Type::Vec4);
        currentOffset = (currentOffset + alignment - 1) & ~(alignment - 1);
        result.layout.push_back({ name, currentOffset, 16, MaterialParameterLayoutEntry::Type::Vec4 });
        result.buffer.resize(currentOffset + 16);
        std::memcpy(result.buffer.data() + currentOffset, &value, 16);
        currentOffset += 16;
    }
    return result;
}

std::vector<std::string> Material::GetTextures() const
{
    return mTextures;
}

IMaterialRenderResources* Material::GetMaterialRenderResources() const
{
    return mGpuHandle.get();
}

void Material::SetFloat(const std::string& name, float value)
{
    mFloatParameters[name] = value;
}

void Material::SetVec3(const std::string& name, glm::vec3 value)
{
    mVec3Parameters[name] = value;
}

void Material::SetVec4(const std::string& name, glm::vec4 value)
{
    mVec4Parameters[name] = value;
}

void Material::AddTexture(const std::string& textureName)
{
    mTextures.push_back(textureName);
}

size_t Material::std140_align(MaterialParameterLayoutEntry::Type type)
{
    switch (type) 
    {
        case MaterialParameterLayoutEntry::Type::Float: return 4;
        case MaterialParameterLayoutEntry::Type::Vec3:  // falls through
        case MaterialParameterLayoutEntry::Type::Vec4:  return 16;
    }
    return 4;
}

size_t Material::std140_size(MaterialParameterLayoutEntry::Type type)
{
    switch (type) 
    {
        case MaterialParameterLayoutEntry::Type::Float: return 4;
        case MaterialParameterLayoutEntry::Type::Vec3:  return 16; // padded to 16
        case MaterialParameterLayoutEntry::Type::Vec4:  return 16;
    }
    return 4;
}

} // namespace rendering_engine