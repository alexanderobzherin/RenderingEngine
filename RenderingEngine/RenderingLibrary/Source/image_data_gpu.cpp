// Copyright (c) 2025 Alexander Obzherin — Licensed under the zlib License. See LICENSE.md.

#include "image_data_gpu.hpp"
#include "image_data.hpp"
#include "i_renderer.hpp"
#include "i_texture_render_resources.hpp"

using namespace rendering_engine;

ImageDataGpu::ImageDataGpu(std::string path, IRenderer* renderer)
	:
	mPath{path},
	mRenderer{renderer},
	mImageData{nullptr},
	mGpuHandle{nullptr}
{
	mImageData = std::make_unique<ImageData>(path);
}

rendering_engine::ImageDataGpu::~ImageDataGpu()
{
}

void ImageDataGpu::UploadToGPU()
{
	if (!mRenderer)
	{
		return;
	}

	if (mGpuHandle)
	{
		if (mGpuHandle->IsTextureLoadedInGPU())
		{
			ReleaseFromGPU();
		}
	}
	mGpuHandle = std::unique_ptr<ITextureRenderResources>(mRenderer->ProvideTextureRenderResources());
	if (!mGpuHandle)
	{
		return;
	}
	mGpuHandle->LoadToGPU(*mImageData);
}

void ImageDataGpu::ReleaseFromGPU()
{
	if (!mRenderer)
	{
		return;
	}
	if (!mGpuHandle)
	{
		return;
	}

	mGpuHandle->ReleaseFromGPU();
	mGpuHandle.release();
	mGpuHandle = nullptr;
}

bool ImageDataGpu::IsOnGPU() const
{
	if (!mGpuHandle)
	{
		return false;
	}
	mGpuHandle->IsTextureLoadedInGPU();
}

size_t rendering_engine::ImageDataGpu::GetSizeInRAM() const
{
	if (!mImageData)
	{
		return 0;
	}

	return mImageData->GetSizeInBytes();
}

size_t rendering_engine::ImageDataGpu::GetSizeInGPU() const
{
	if (!mGpuHandle)
	{
		return 0;
	}

	return mGpuHandle->GetSizeInGPUBytes();
}

ImageData const& ImageDataGpu::GetCpuImageData() const
{
	return *mImageData;
}

ImageData& ImageDataGpu::GetCpuImageData()
{
	return *mImageData;
}

ITextureRenderResources* ImageDataGpu::GetTextureRenderResources() const
{
	return mGpuHandle.get();
}
