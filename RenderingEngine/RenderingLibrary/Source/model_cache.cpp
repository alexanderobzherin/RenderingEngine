#include "model_cache.hpp"
#include "i_renderer.hpp"
#include "boost/filesystem.hpp"
#include "mesh_data_gpu.hpp"
#include "utility.hpp"

namespace rendering_engine
{

ModelCache::ModelCache(IRenderer* renderer)
	:
	mRenderer(renderer),
	mTotalSizeRAM(0),
	mTotalSizeGPU(0)
{
	mRenderer->RegisterObserver(this);
}

ModelCache::~ModelCache()
{
	mRenderer->UnregisterObserver(this);
}

void ModelCache::LoadModelsFromFolder(std::string pathToFolder)
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
		(void)UploadModelToRAM(x.path().string());
	}
}

void ModelCache::LoadModelsFromPackage()
{
	const auto& entries = Utility::GetPackEntries();

	std::string folderEntry = { "Models/" };
	for (auto& entry : entries)
	{
		const std::string& virtualPath = entry.first;
		if (virtualPath.rfind(folderEntry, 0) == 0) // starts with Models/
		{
			std::string modelName = virtualPath.substr(folderEntry.size());

			std::vector<uint8_t> binaryFileData = Utility::ReadPackedFile(virtualPath);
			if (binaryFileData.empty())
			{
				std::cerr << "[TextureCache] Could not read packed texture: "
					<< virtualPath << std::endl;
				continue;
			}

			// Upload to RAM with textureName + binaryFileData
			(void)UploadModelToRAM(modelName, binaryFileData);
		}
	}
}

void ModelCache::CreateQuad2D()
{
	mModels["Quad2D"] = std::make_shared<MeshDataGpu>(mRenderer);
	mModels["Quad2D"]->CreateQuad2D();

	const size_t sizeVertices = mModels.at("Quad2D")->GetCpuVertexBufferSize();
	mTotalSizeRAM += sizeVertices;
	const size_t sizeIndices = mModels.at("Quad2D")->GetCpuIndexBufferSize();
	mTotalSizeRAM += sizeIndices;
}

void ModelCache::LoadCustomMesh(std::string meshName, std::vector<glm::vec2> positions2D, std::vector<glm::vec2> texCoords, std::vector<glm::vec4> colors, std::vector<std::uint32_t> indices)
{
	auto search = mModels.find(meshName);
	if (search != mModels.end())
	{
		mTotalSizeRAM -= search->second->GetCpuVertexBufferSize();
		mTotalSizeRAM -= search->second->GetCpuIndexBufferSize();
		ReleaseModelFromGPU(meshName);
	}

	mModels[meshName] = std::make_shared<MeshDataGpu>(mRenderer);
	mModels[meshName]->LoadCustomMesh(positions2D, texCoords, colors, indices);

	const size_t sizeVertices = mModels.at(meshName)->GetCpuVertexBufferSize();
	mTotalSizeRAM += sizeVertices;
	const size_t sizeIndices = mModels.at(meshName)->GetCpuIndexBufferSize();
	mTotalSizeRAM += sizeIndices;
}

std::string ModelCache::UploadModelToRAM(std::string path)
{
	auto filePath = boost::filesystem::path(path);
	if (!boost::filesystem::is_regular_file(filePath))
	{
		return std::string{};
	}

	const std::string ext = filePath.extension().string();
	const bool isExtensionSupported = (ext == ".fbx");
	if (!isExtensionSupported)
	{
		return std::string{};
	}

	std::string filename = filePath.stem().string();
	// If model is already loaded into RAM yet, do not add again.
	if (auto search = mModels.find(filename); search != mModels.end())
	{
		return std::string{};
	}
	mModels[filename] = std::make_shared<MeshDataGpu>(filePath.string(), mRenderer);

	const size_t sizeVertices = mModels.at(filename)->GetCpuVertexBufferSize();
	mTotalSizeRAM += sizeVertices;
	const size_t sizeIndices = mModels.at(filename)->GetCpuIndexBufferSize();
	mTotalSizeRAM += sizeIndices;

	return filename;
}

std::string ModelCache::UploadModelToRAM(std::string fileName, std::vector<uint8_t> const& fileBytes)
{
	auto modelName = boost::filesystem::path(fileName).stem().string();

	// If model is already loaded into RAM yet, do not add again.
	if (auto search = mModels.find(modelName); search != mModels.end())
	{
		return std::string{};
	}

	mModels[modelName] = std::make_shared<MeshDataGpu>(fileBytes, mRenderer);

	const size_t sizeVertices = mModels.at(modelName)->GetCpuVertexBufferSize();
	mTotalSizeRAM += sizeVertices;
	const size_t sizeIndices = mModels.at(modelName)->GetCpuIndexBufferSize();
	mTotalSizeRAM += sizeIndices;

	return modelName;
}

void ModelCache::UploadModelToGPU(std::string filename)
{
	// If texture is not loaded in RAM yet, skip loading to GPU.
	if (auto search = mModels.find(filename); search == mModels.end())
	{
		return;
	}
	if (mModels[filename]->IsOnGPU())
	{
		return;
	}

	mModels[filename]->UploadToGPU();
	size_t sizeVertices = mModels[filename]->GetGpuVertexBufferSize();
	mTotalSizeGPU += sizeVertices;
	size_t sizeIndices = mModels[filename]->GetGpuIndexBufferSize();
	mTotalSizeGPU += sizeIndices;
}

void ModelCache::ReleaseModelFromGPU(std::string filename)
{
	if (auto search = mModels.find(filename); search == mModels.end())
	{
		return;
	}

	auto& model = mModels[filename];
	size_t sizeVertices = model->GetGpuVertexBufferSize();
	size_t sizeIndices = model->GetGpuIndexBufferSize();
	model->ReleaseFromGPU();

	mTotalSizeGPU -= sizeVertices;
	mTotalSizeGPU -= sizeIndices;
}

void ModelCache::ReleaseAllFromGPU()
{
	for (auto& texture : mModels)
	{
		texture.second->ReleaseFromGPU();
	}
	mTotalSizeGPU = 0;
}

void ModelCache::ReleaseAll()
{
	mModels.clear();
	mTotalSizeRAM = 0;
	mTotalSizeGPU = 0;
}

std::shared_ptr<MeshDataGpu> ModelCache::GetMeshResources(std::string filename)
{
	auto search = mModels.find(filename);
	if (search == mModels.end())
	{
		return nullptr;
	}
	return search->second;
}

IMeshRenderResources* ModelCache::GetMeshRenderResources(std::string filename)
{
	if (auto search = mModels.find(filename); search == mModels.end())
	{
		return nullptr;
	}

	return mModels.at(filename)->GetMeshRenderResources();
}

size_t ModelCache::GetSizeInRAM() const
{
	return mTotalSizeRAM;
}

size_t ModelCache::GetSizeInGPU() const
{
	return mTotalSizeGPU;
}

void ModelCache::OnRenderResourcesRelease()
{
	ReleaseAllFromGPU();
}

void ModelCache::OnRenderResourcesRebuild()
{
	for (auto& model : mModels)
	{
		model.second->UploadToGPU();
		size_t sizeVertices = model.second->GetGpuVertexBufferSize();
		mTotalSizeGPU += sizeVertices;
		size_t sizeIndices = model.second->GetGpuIndexBufferSize();
		mTotalSizeGPU += sizeIndices;
	}
}

} // namespace rendering_engine
