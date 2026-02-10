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
    mGpuHandle(nullptr),
    mParameterLayout(matSettings.parameterLayout)
{}
Material::~Material()
{
    if (mGpuHandle)
        ReleaseRenderResources();
}
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
    if (!mGpuHandle)
        return;

    mGpuHandle->Shutdown();
    mGpuHandle.reset();
}
PackedMaterialData Material::PackMaterialParameters()
{
    PackedMaterialData result;

    if (!mParameterLayout)
        return result;

    // Preallocate full buffer size
    size_t totalSize = 0;
    for (const auto& e : *mParameterLayout)
        totalSize = std::max(totalSize, e.offset + e.size);

    result.buffer.resize(totalSize);
    result.layout = *mParameterLayout;

    for (const auto& entry : *mParameterLayout)
    {
        uint8_t* dst = result.buffer.data() + entry.offset;

        switch (entry.type)
        {
            case MaterialParameterLayoutEntry::Type::Float:
                std::memcpy(dst, &mFloatParameters[entry.name], 4);
                break;

            case MaterialParameterLayoutEntry::Type::Vec2:
                std::memcpy(dst, &mVec2Parameters[entry.name], 8);
                break;

            case MaterialParameterLayoutEntry::Type::Vec3:
                std::memcpy(dst, &mVec3Parameters[entry.name], 12);
                std::memset(dst + 12, 0, 4); // std140 padding
                break;

            case MaterialParameterLayoutEntry::Type::Vec4:
                std::memcpy(dst, &mVec4Parameters[entry.name], 16);
                break;
        }
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

void Material::SetVec2(const std::string& name, glm::vec2 value)
{
    mVec2Parameters[name] = value;
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
        case MaterialParameterLayoutEntry::Type::Vec2:  return 8;
        case MaterialParameterLayoutEntry::Type::Vec3:  // fallthrough
        case MaterialParameterLayoutEntry::Type::Vec4:  return 16;
    }
    return 4;
}

size_t Material::std140_size(MaterialParameterLayoutEntry::Type type)
{
    switch (type) 
    {
        case MaterialParameterLayoutEntry::Type::Float: return 4;
        case MaterialParameterLayoutEntry::Type::Vec2:  return 8;
        case MaterialParameterLayoutEntry::Type::Vec3:  return 16;
        case MaterialParameterLayoutEntry::Type::Vec4:  return 16;
    }
    return 4;
}

} // namespace rendering_engine