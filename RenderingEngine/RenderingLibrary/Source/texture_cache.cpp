// Copyright (c) 2026 Alexander Obzherin � Licensed under the zlib License. See LICENSE.md.

#include "texture_cache.hpp"
#include "image_data_gpu.hpp"
#include "image_data.hpp"
#include "i_renderer.hpp"
#include <string>
#include <vector>
#include "boost/filesystem.hpp"
#include "utility.hpp"
#include "logger.hpp"

namespace rendering_engine
{

TextureCache::TextureCache(IRenderer* renderer)
	:
	mRenderer(renderer),
	mTotalSizeRAM(0),
	mTotalSizeGPU(0)
{
	LOG_INFO("TextureCache created.");
	mRenderer->RegisterObserver(this);
}

TextureCache::~TextureCache()
{
	LOG_DEBUG("TextureCache destroyed.");
	mRenderer->UnregisterObserver(this);
}

void TextureCache::LoadTexturesFromFolder(std::string pathToFolder)
{
	LOG_INFO("Loading textures from folder: " + pathToFolder);
	auto start = std::chrono::steady_clock::now();
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

	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("Loaded " + std::to_string(mTextures.size()) +
		" textures from folder in " +
		std::to_string(ms) + " ms. RAM usage: " +
		std::to_string(mTotalSizeRAM) + " bytes.");
}

void TextureCache::LoadTexture(std::string textureName, ImageData imageData)
{
	// Upload to RAM with textureName + binaryFileData
	auto textureNameUploaded = UploadTextureToRAM(textureName, imageData);
	if (!textureNameUploaded.empty())
	{
		UploadTextureToGPU(textureNameUploaded);
	}
}

void TextureCache::LoadTexturesFromPackage()
{
	LOG_INFO("Loading textures from package.");
	auto start = std::chrono::steady_clock::now();
	const auto& entries = Utility::GetPackEntries();

	std::string folderEntry = { "Textures/" };
	for (auto& entry : entries)
	{
		const std::string& virtualPath = entry.first;
		if (virtualPath.rfind(folderEntry, 0) == 0) // starts with Textures/
		{
			std::string textureFileName = virtualPath.substr(folderEntry.size());

			std::vector<uint8_t> binaryFileData = Utility::ReadPackedFile(virtualPath);
			if (binaryFileData.empty())
			{
				LOG_ERROR("[TextureCache] Could not read packed texture: "
					+ virtualPath);
				continue;
			}

			// Upload to RAM with textureName + binaryFileData
			auto textureName = UploadTextureToRAM(textureFileName, binaryFileData);
			if (!textureName.empty())
			{
				UploadTextureToGPU(textureName);
			}
		}
	}
	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("Loaded " + std::to_string(mTextures.size()) +
		" textures from package in " +
		std::to_string(ms) + " ms. RAM usage: " +
		std::to_string(mTotalSizeRAM) + " bytes.");
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
	LOG_DEBUG("Uploading texture to RAM: " + filename);
	auto start = std::chrono::steady_clock::now();
	mTextures[filename] = std::make_shared<ImageDataGpu>(path, mRenderer);

	size_t size = mTextures.at(filename)->GetSizeInRAM();
	mTotalSizeRAM += size;

	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_DEBUG("Texture loaded to RAM: " + filename +
		" (" + std::to_string(size) +
		" bytes, " + std::to_string(ms) + " ms)");

	return filename;
}

std::string TextureCache::UploadTextureToRAM(std::string textureFileName, std::vector<uint8_t> const& fileBytes)
{
	auto textureName = boost::filesystem::path(textureFileName).stem().string();
	// If texture is already loaded into RAM yet, do not add again.
	if (auto search = mTextures.find(textureName); search != mTextures.end())
	{
		return std::string{};
	}
	LOG_DEBUG("Uploading texture to RAM: " + textureName);
	auto start = std::chrono::steady_clock::now();
	mTextures[textureName] = std::make_shared<ImageDataGpu>(fileBytes, mRenderer);

	size_t size = mTextures.at(textureName)->GetSizeInRAM();
	mTotalSizeRAM += size;
	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_DEBUG("Texture loaded to RAM: " + textureName +
		" (" + std::to_string(size) +
		" bytes, " + std::to_string(ms) + " ms)");

	return textureName;
}

std::string TextureCache::UploadTextureToRAM(std::string textureName, ImageData imageData)
{
	// If texture is already loaded into RAM yet, do not add again.
	if (auto search = mTextures.find(textureName); search != mTextures.end())
	{
		return std::string{};
	}
	LOG_DEBUG("Uploading texture to RAM: " + textureName);
	auto start = std::chrono::steady_clock::now();
	mTextures[textureName] = std::make_shared<ImageDataGpu>(imageData, mRenderer);

	size_t size = mTextures.at(textureName)->GetSizeInRAM();
	mTotalSizeRAM += size;
	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_DEBUG("Texture loaded to RAM: " + textureName +
		" (" + std::to_string(size) +
		" bytes, " + std::to_string(ms) + " ms)");

	return textureName;
}

void TextureCache::UploadTextureToGPU(std::string textureName)
{
	LOG_DEBUG("Uploading texture to GPU: " + textureName);
	auto start = std::chrono::steady_clock::now();
	// If texture is not loaded in RAM yet, skip loading to GPU.
	if (auto search = mTextures.find(textureName); search == mTextures.end())
	{
		return;
	}
	if (mTextures[textureName]->IsOnGPU())
	{
		return;
	}

	mTextures[textureName]->UploadToGPU();
	size_t size = mTextures[textureName]->GetSizeInGPU();
	mTotalSizeGPU += size;

	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_DEBUG("Texture uploaded to GPU: " + textureName +
		" (" + std::to_string(size) +
		" bytes, " + std::to_string(ms) + " ms)");
}
	
void TextureCache::ReleaseTextureFromGPU(std::string textureName)
{
	LOG_DEBUG("Releasing texture from GPU: " + textureName);
	if (auto search = mTextures.find(textureName); search == mTextures.end())
	{
		return;
	}

	auto& texture = mTextures[textureName];
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
	LOG_INFO("Releasing all textures. RAM usage before clear: " +
		std::to_string(mTotalSizeRAM) +
		", GPU usage: " +
		std::to_string(mTotalSizeGPU));
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