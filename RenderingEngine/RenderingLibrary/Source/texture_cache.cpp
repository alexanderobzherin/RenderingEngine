// Copyright (c) 2025 Alexander Obzherin — Licensed under the zlib License. See LICENSE.md.

#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "i_renderer.hpp"
#include <string>
#include <vector>
#include "boost/filesystem.hpp"

namespace rendering_engine
{

TextureCache::TextureCache(IRenderer* renderer)
	:
	mRenderer(renderer),
	mTotalSizeRAM(0),
	mTotalSizeGPU(0)
{
	mRenderer->RegisterObserver(this);
}

TextureCache::~TextureCache()
{
	mRenderer->UnregisterObserver(this);
}

void TextureCache::LoadTexturesFromFolder(std::string pathToFolder)
{
	// 1. Check if path is valid and exist
	boost::filesystem::path pathToDirectory = boost::filesystem::path(pathToFolder);
	const bool isValidFolderPath = boost::filesystem::exists(boost::filesystem::path(pathToFolder)) && boost::filesystem::is_directory(boost::filesystem::path(pathToFolder));
	if (!isValidFolderPath)
	{
		return;
	}
	// 2. Iterate through files in the folder.
	//    if file is in the list of supported extensions
	for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(pathToDirectory))
	{
		auto textureName = UploadTextureToRAM(x.path().string());
		if (!textureName.empty())
		{
			UploadTextureToGPU(textureName);
		}
	}
}

std::string TextureCache::UploadTextureToRAM(std::string path)
{
	auto filePath = boost::filesystem::path(path);
	if (!boost::filesystem::is_regular_file(filePath))
	{
		return std::string{};
	}
	
	const std::string ext = filePath.extension().string();
	const bool isExtensionSupported = (ext == ".jpg") || (ext == ".png");
	if (!isExtensionSupported)
	{
		return std::string{};
	}

	std::string filename = filePath.stem().string();
	// If texture is already loaded into RAM yet, do not add again.
	if (auto search = mTextures.find(filename); search != mTextures.end())
	{
		return std::string{};
	}
	mTextures[filename] = std::make_shared<ImageDataGpu>(path, mRenderer);

	size_t size = mTextures.at(filename)->GetSizeInRAM();
	mTotalSizeRAM += size;

	return filename;
}

void TextureCache::UploadTextureToGPU(std::string filename)
{
	// If texture is not loaded in RAM yet, skip loading to GPU.
	if (auto search = mTextures.find(filename); search == mTextures.end())
	{
		return;
	}
	if (mTextures[filename]->IsOnGPU())
	{
		return;
	}

	mTextures[filename]->UploadToGPU();
	size_t size = mTextures[filename]->GetSizeInGPU();
	mTotalSizeGPU += size;
}
	
void TextureCache::ReleaseTextureFromGPU(std::string filename)
{
	if (auto search = mTextures.find(filename); search == mTextures.end())
	{
		return;
	}

	auto& texture = mTextures[filename];
	size_t size = texture->GetSizeInGPU();
	texture->ReleaseFromGPU();

	mTotalSizeGPU -= size;
}

void TextureCache::ReleaseAllFromGPU()
{
	for (auto& texture : mTextures)
	{
		texture.second->ReleaseFromGPU();
	}
	mTotalSizeGPU = 0;
}

void TextureCache::ReleaseAll()
{
	ReleaseAllFromGPU();
	mTextures.clear();
	mTotalSizeRAM = 0;
	mTotalSizeGPU = 0;
}

std::shared_ptr<ImageDataGpu> TextureCache::GetTextureResources(std::string filename)
{
	auto search = mTextures.find(filename);
	if (search == mTextures.end())
	{
		return nullptr;
	}
	return search->second;
}

ITextureRenderResources* TextureCache::GetTextureRenderResources(std::string filename)
{
	if (auto search = mTextures.find(filename); search == mTextures.end())
	{
		return nullptr;
	}

	return mTextures.at(filename)->GetTextureRenderResources();
}

size_t TextureCache::GetSizeInRAM() const
{
	return mTotalSizeRAM;
}

size_t TextureCache::GetSizeInGPU() const
{
	return mTotalSizeGPU;
}

void TextureCache::OnRenderResourcesRelease()
{
	ReleaseAllFromGPU();
}

void TextureCache::OnRenderResourcesRebuild()
{
	for (auto& texture : mTextures)
	{
		texture.second->UploadToGPU();
		size_t size = texture.second->GetSizeInGPU();
		mTotalSizeGPU += size;
	}
}

}