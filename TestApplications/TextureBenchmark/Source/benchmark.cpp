#include "..\Include\benchmark.hpp"
#include "logger.hpp"
#include "image_data.hpp"
#include "utility.hpp"

#include "boost/filesystem.hpp"
#include <chrono>

using namespace rendering_engine;

namespace test_app
{

Benchmark::Benchmark()
{
}

void Benchmark::BenchmarkTextureLoadAndCopy(size_t numOfIterations)
{
	const size_t numberOfIterations = (numOfIterations > 0) ? numOfIterations : DefaultNumOfIterations;

	const auto textureFolder = Utility::GetTextureFolderPath();

	boost::filesystem::path pathToDirectory = boost::filesystem::path(textureFolder.string());
	const bool isValidFolderPath = boost::filesystem::exists(boost::filesystem::path(textureFolder)) && boost::filesystem::is_directory(boost::filesystem::path(textureFolder));
	if (!isValidFolderPath)
	{
		return;
	}

	std::vector<TextureLoadSummary> summary;
	for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(pathToDirectory))
	{
		auto filePath = boost::filesystem::path(x.path());
		const std::string ext = filePath.extension().string();
		const bool isExtensionSupported = (ext == ".png") || (ext == ".jpg" || (ext == ".jpeg"));
		if (!boost::filesystem::is_regular_file(filePath) || !isExtensionSupported)
		{
			continue;
		}
		LoadTextureFile(x.path().string()); // warm-up, ignore result

		std::vector<TextureLoadMetrics> metrics;
		metrics.reserve(numberOfIterations);
		for (size_t it = 0; it < numberOfIterations; ++it)
		{
			TextureLoadMetrics textureLoadMetrics = LoadTextureFile(x.path().string());
			metrics.push_back(textureLoadMetrics);
	
			std::cout << "Texture: " << textureLoadMetrics.file << " | " <<
				std::to_string(textureLoadMetrics.width) << "x" << std::to_string(textureLoadMetrics.height) << " | " <<
				std::to_string(textureLoadMetrics.sizeBytes) << " bytes" << " | " <<
				"Load: " << std::to_string(textureLoadMetrics.loadTimeMs) << " ms " << " | " <<
				"Copy: " << std::to_string(textureLoadMetrics.copyTimeMs) << " ms "
				<< std::endl;
		}
		summary.push_back(ComputeTextureSummary(metrics));
	}

	std::cout << std::endl;

	PrintTextureTestSummary(summary);
}

TextureLoadMetrics Benchmark::LoadTextureFile(std::string const& texturePath)
{
	TextureLoadMetrics textureLoadMetrics;
	auto startTimeLoad = std::chrono::steady_clock::now();

	ImageData testImageData(texturePath);

	auto endTimeLoad = std::chrono::steady_clock::now();
	auto durationLoad = std::chrono::duration<double, std::milli>(endTimeLoad - startTimeLoad).count();
	
	auto startTimeCopy = std::chrono::steady_clock::now();

	ImageData testImageDataCopied = testImageData;

	auto endTimeCopy = std::chrono::steady_clock::now();
	auto durationCopy = std::chrono::duration<double, std::milli>(endTimeCopy - startTimeCopy).count();

	auto path = boost::filesystem::path(texturePath);
	
	textureLoadMetrics.file = path.filename().string();
	textureLoadMetrics.loadTimeMs = durationLoad;
	textureLoadMetrics.copyTimeMs = durationCopy;
	textureLoadMetrics.width = testImageData.GetWidth();
	textureLoadMetrics.height = testImageData.GetHeight();
	textureLoadMetrics.sizeBytes = testImageData.GetSizeInBytes();

	return textureLoadMetrics;
}

TextureLoadSummary Benchmark::ComputeTextureSummary(const std::vector<TextureLoadMetrics>& metrics)
{
	TextureLoadSummary textureLoadSummary;

	if (metrics.empty())
	{
		return textureLoadSummary;
	}
	textureLoadSummary.file = metrics[0].file;
	textureLoadSummary.width = metrics[0].width;
	textureLoadSummary.height = metrics[0].height;
	textureLoadSummary.sizeBytes = metrics[0].sizeBytes;
	textureLoadSummary.loadAvgMs = metrics[0].loadTimeMs;
	textureLoadSummary.loadMinMs = metrics[0].loadTimeMs;
	textureLoadSummary.loadMaxMs = metrics[0].loadTimeMs;
	textureLoadSummary.copyAvgMs = metrics[0].copyTimeMs;
	textureLoadSummary.copyMinMs = metrics[0].copyTimeMs;
	textureLoadSummary.copyMaxMs = metrics[0].copyTimeMs;

	double loadAccumulativeMs = 0.0;
	double copyAccumulativeMs = 0.0;
	for (const auto& it : metrics)
	{
		if (it.loadTimeMs < textureLoadSummary.loadMinMs)
		{
			textureLoadSummary.loadMinMs = it.loadTimeMs;
		}
		if (it.loadTimeMs > textureLoadSummary.loadMaxMs)
		{
			textureLoadSummary.loadMaxMs = it.loadTimeMs;
		}
		loadAccumulativeMs += it.loadTimeMs;

		if (it.copyTimeMs < textureLoadSummary.copyMinMs)
		{
			textureLoadSummary.copyMinMs = it.copyTimeMs;
		}
		if (it.copyTimeMs > textureLoadSummary.copyMaxMs)
		{
			textureLoadSummary.copyMaxMs = it.copyTimeMs;
		}
		copyAccumulativeMs += it.copyTimeMs;
	}

	textureLoadSummary.loadAvgMs = loadAccumulativeMs / static_cast<double>(metrics.size());
	textureLoadSummary.copyAvgMs = copyAccumulativeMs / static_cast<double>(metrics.size());

	return textureLoadSummary;
}

void Benchmark::PrintTextureTestSummary(const std::vector<TextureLoadSummary>& summary)
{
	std::cout << "*************************** Texture Load Copy Summary ***************************" << std::endl;
	std::cout << std::endl;

	for (const auto& texture : summary)
	{
		std::cout << "Texture: " << texture.file << " | " <<
			std::to_string(texture.width) << "x" << std::to_string(texture.height) << " | " <<
			std::to_string(texture.sizeBytes) << " bytes" << " | " <<
			"Load avg/min/max: " << std::to_string(texture.loadAvgMs) << " / " << std::to_string(texture.loadMinMs) << " / " <<  std::to_string(texture.loadMaxMs) << " ms" << " | " <<
			"Copy avg/min/max: " << std::to_string(texture.copyAvgMs) << " / " << std::to_string(texture.copyMinMs) << " / " << std::to_string(texture.copyMaxMs) << " ms"
			<< std::endl;
	}
}

}