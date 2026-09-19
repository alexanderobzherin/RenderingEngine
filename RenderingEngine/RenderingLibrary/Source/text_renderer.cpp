#include "text_renderer.hpp"
#include "render_resource_context.hpp"
#include "font_resources.hpp"
#include "logger.hpp"

namespace rendering_engine
{
std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> TextRenderer::sScriptRanges{
	// European
	{{"Latin"}, {0x0020U, 0x007EU}},
	{{"Cyrillic"}, {0x0400U, 0x04FFU}},
	{{"Greek"}, {0x0370U, 0x03FFU}},

	// Asian
	{{"Han"}, {0x4E00U, 0x9FFFU}},
	{{"HanExtensionA"}, {0x3400U, 0x4DBFU}},

	{{"Hiragana"}, {0x3040U, 0x309FU}},
	{{"Katakana"}, {0x30A0U, 0x30FFU}},
	{{"KatakanaPhoneticExtensions"}, {0x31F0U, 0x31FFU}},

	{{"Hangul"}, {0xAC00U, 0xD7AFU}},

	// Requires shaping
	// East
	{{"Hebrew"}, {0x0590U, 0x05FFU}},
	{{"Arabic"}, {0x0600U, 0x06FFU}},
	{{"ArabicSupplement"}, {0x0750U, 0x077FU}},
	{{"ArabicExtended-A"}, {0x08A0U, 0x08FFU}},
	{{"Aramaic"}, {0x0700U, 0x074FU}},
	{{"Thaana"}, {0x0780U, 0x07BFU}},

	// Indic
	{{"Devanagari"}, {0x0900U, 0x097FU}},
	{{"Bengali"}, {0x0980U, 0x09FFU}},
	{{"Gurmukhi"}, {0x0A00U, 0x0A7FU}},
	{{"Gujarati"}, {0x0A80U, 0x0AFFU}},
	{{"Oriya"}, {0x0B00U, 0x0B7FU}},
	{{"Tamil"}, {0x0B80U, 0x0BFFU}},
	{{"Telugu"}, {0x0C00U, 0x0C7FU}},
	{{"Kannada"}, {0x0C80U, 0x0CFFU}},
	{{"Malayalam"}, {0x0D00U, 0x0D7FU}},
	{{"Sinhala"}, {0x0D80U, 0x0DFFU}},

	// Southeast Asian
	{{"Thai"}, {0x0E00U, 0x0E7FU}},
	{{"Lao"}, {0x0E80U, 0x0EFFU}},
	{{"Myanmar"}, {0x1000U, 0x109FU}},
	{{"Khmer"}, {0x1780U, 0x17FFU}},

	// Tibetan
	{{"Tibetan"}, {0x0F00U, 0x0FFFU}}
};
std::vector<std::string> TextRenderer::sFontAtlasPreloadableScripts{
	// European
	{"Latin"},			
	{"Cyrillic"},		
	{"Greek"},			

	// Asian
	{"Han"},			
	{"HanExtensionA"}, 

	{"Hiragana"},		
	{"Katakana"},		
	{"KatakanaPhoneticExtensions"},

	{"Hangul"}
};

std::vector<std::string> TextRenderer::sScriptsRequiresShaping{
	{"Hebrew"}, 
	{"Arabic"},
	{"ArabicSupplement"},
	{"ArabicExtended-A"},
	{"Aramaic"},
	{"Thaana"},
	{"Devanagari"},
	{"Bengali"},
	{"Gurmukhi"},
	{"Gujarati"},
	{"Oriya"},
	{"Tamil"},
	{"Telugu"},
	{"Kannada"},
	{"Malayalam"},
	{"Sinhala"},
	{"Thai"},
	{"Lao"},
	{"Myanmar"},
	{"Khmer"},
	{"Tibetan"}
};

TextRenderer::TextRenderer(RenderResourceContext rrc)
	:
	mRenderResourceContext(rrc)
{
	LOG_INFO("Initializing TextRenderer...");
	auto start = std::chrono::steady_clock::now();

	mErrorResult = FT_Init_FreeType(&mLibrary);
	if (mErrorResult)
	{
		LOG_ERROR("Failed to initialize FreeType library!");
		throw std::runtime_error{ "Failed to initialize FreeType library!" };
	}

	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("FreeType initialized in " + std::to_string(ms) + " ms.");
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::Shutdown()
{
	LOG_INFO("Shutting down TextRenderer...");
    if (!mLibrary)
        return;

    for (auto& fontResource : mFontResources)
        fontResource.second.reset();

    mFontResources.clear();

    FT_Done_FreeType(mLibrary);
    mLibrary = nullptr;
	LOG_INFO("TextRenderer shutdown complete.");
}

void TextRenderer::LoadFontsFromFolder(std::string pathToFolder)
{
	LOG_INFO("Loading fonts from folder: " + pathToFolder);
	auto start = std::chrono::steady_clock::now();
	LoadFontsAvailableInFolder(pathToFolder);
	LoadPreloadableFontAtlasesFromFolder(mAvailableFontsInFolder);
	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("Font discovery and preload complete in " +
		std::to_string(ms) + " ms.");
}

void TextRenderer::LoadFontsFromPackage()
{
	LoadFontsAvailableInPackage();
	LoadPreloadableFontAtlasesFromPackage(mAvailableFontsInPackage);
}

const RenderResourceContext& TextRenderer::GetRenderResourceContext() const
{
	return mRenderResourceContext;
}

std::shared_ptr<FontResources> TextRenderer::GetFontResources(const std::string& fontName, unsigned int fontSize)
{
	auto key = std::make_pair(fontName, fontSize);
	if (auto search = mFontResources.find(key); search != mFontResources.end())
	{
		return search->second;
	}
	else
	{
		if (!mAvailableFontsInPackage.empty())
		{
			if (!mAvailableFontsInPackage.empty())
			{
				auto foundInPackage = mAvailableFontsInPackage.find(fontName);
				if (foundInPackage != mAvailableFontsInPackage.end())
				{
					const std::string virtualFilePath = mAvailableFontsInPackage[fontName];
					std::vector<uint8_t> binaryFileData = Utility::ReadPackedFile(virtualFilePath);

					mFontResources[key] = std::make_shared<FontResources>(mRenderResourceContext, this, fontName, binaryFileData, fontSize);
					mFontResources[key]->StoreFontAtlasesInFiles(bStoreFontAtlasesInFiles);

					return mFontResources[key];
				}
				else
				{
					// Log Font {fontName} is not avalable in asset package
				}
			}
		}
		else
		{
			if (!mAvailableFontsInFolder.empty())
			{
				auto foundInFolder = mAvailableFontsInFolder.find(fontName);
				if (foundInFolder != mAvailableFontsInFolder.end())
				{
					auto key = std::make_pair(fontName, fontSize);
					mFontResources[key] = std::make_shared<FontResources>(mRenderResourceContext, this, mAvailableFontsInFolder[fontName], fontSize);

					return mFontResources[key];
				}
				else
				{
					// Log Font {fontName} is not avalable in asset folder
				}
			}
		}
	}

	return nullptr;
}

void TextRenderer::StoreFontAtlasesInFiles(bool in)
{
	bStoreFontAtlasesInFiles = in;
}

const std::vector<std::string>& TextRenderer::GetScriptsRequiredShaping() const
{
	return sScriptsRequiresShaping;
}

std::pair<std::uint32_t, std::uint32_t> TextRenderer::GetScriptRange(std::string script)
{
	std::pair<std::uint32_t, std::uint32_t> result{ 0U, 0U };

	auto search = sScriptRanges.find(script);
	{
		if (search != sScriptRanges.end())
			return sScriptRanges[script];
	}
	return result;
}

void TextRenderer::LoadFontsAvailableInFolder(std::string pathToFolder)
{
	LOG_INFO("Scanning font folder: " + pathToFolder);
	// 1. Check if path is valid and exist
	std::filesystem::path pathToDirectory = std::filesystem::path(pathToFolder);
	const bool isValidFolderPath = std::filesystem::exists(std::filesystem::path(pathToFolder)) && std::filesystem::is_directory(std::filesystem::path(pathToFolder));
	if (!isValidFolderPath)
	{
		return;
	}
	// 2. Iterate through files in the folder.
	//    if file is in the list of supported extensions
	for (std::filesystem::recursive_directory_iterator it(pathToDirectory), end;
		it != end;
		++it)
	{
		const std::filesystem::path& filePath = it->path();

		if (!std::filesystem::is_regular_file(filePath))
			continue;

		const std::string ext = filePath.extension().string();
		if (ext != ".ttf" && ext != ".otf")
			continue;

		// Fonts name and file path stored for future runtime loading
		std::string fontName = filePath.stem().string();
		mAvailableFontsInFolder[fontName] = filePath.string();
	}
	LOG_INFO("Discovered " +
		std::to_string(mAvailableFontsInFolder.size()) +
		" fonts in folder.");
}

void TextRenderer::LoadPreloadableFontAtlasesFromFolder(const std::unordered_map<std::string, std::string>& availableFontsInFolder)
{
	LOG_INFO("Preloading font atlases from folder...");
	auto start = std::chrono::steady_clock::now();
	AppConfig appConfig = Utility::ReadConfigFile();

	for (const auto& [fontName, filePath] : availableFontsInFolder)
	{
		for (auto requestedScript : appConfig.textScripts)
		{
			auto preloadableScriptIt = std::find(sFontAtlasPreloadableScripts.begin(), sFontAtlasPreloadableScripts.end(), requestedScript);
			if (preloadableScriptIt != sFontAtlasPreloadableScripts.end())
			{
				for (auto fontSize : appConfig.fontSizePreload)
				{
					auto key = std::make_pair(fontName, fontSize);
					if (mFontResources.find(key) == mFontResources.end())
					{
						LOG_DEBUG("Creating font resource: " + fontName +
							" size " + std::to_string(fontSize));
						mFontResources[key] = std::make_shared<FontResources>(mRenderResourceContext, this, filePath, fontSize);
						mFontResources[key]->StoreFontAtlasesInFiles(bStoreFontAtlasesInFiles);
					}
					LOG_DEBUG("Preloading script '" + requestedScript +
						"' for font " + fontName +
						" size " + std::to_string(fontSize));
					const std::uint32_t rangeBegin = sScriptRanges[requestedScript].first;
					const std::uint32_t rangeEnd = sScriptRanges[requestedScript].second;
					mFontResources[key]->LoadGlyphsFromCodePointRange(rangeBegin, rangeEnd);
				}
			}
		}
	}

	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("Font atlas preloading (folder) completed in " +
		std::to_string(ms) + " ms.");
}

void TextRenderer::LoadFontsAvailableInPackage()
{
	LOG_INFO("Scanning fonts in package...");
	const auto& entries = Utility::GetPackEntries();

	std::string folderEntry = { "Fonts/" };
	for (auto& entry : entries)
	{
		const std::string& virtualPath = entry.first;
		if (virtualPath.rfind(folderEntry, 0) == 0) // starts with Fonts/
		{
			auto fontFilePath = std::filesystem::path(virtualPath);
			const std::string ext = fontFilePath.extension().string();
			if (ext != ".ttf" && ext != ".otf")
				continue;

			// Fonts name and virtual path stored for future runtime loading
			std::string fontName = fontFilePath.stem().string();
			mAvailableFontsInPackage[fontName] = fontFilePath.string();
		}
	}
	LOG_INFO("Discovered " +
		std::to_string(mAvailableFontsInPackage.size()) +
		" fonts in package.");
}

void TextRenderer::LoadPreloadableFontAtlasesFromPackage(const std::unordered_map<std::string, std::string>& availableFontsInPackage)
{
	LOG_INFO("Preloading font atlases from package...");
	auto start = std::chrono::steady_clock::now();
	AppConfig appConfig = Utility::ReadConfigFile();

	for (const auto& [fontName, virtualFilePath] : availableFontsInPackage)
	{
		for (auto requestedScript : appConfig.textScripts)
		{
			auto preloadableScriptIt = std::find(sFontAtlasPreloadableScripts.begin(), sFontAtlasPreloadableScripts.end(), requestedScript);
			if (preloadableScriptIt != sFontAtlasPreloadableScripts.end())
			{
				for (auto fontSize : appConfig.fontSizePreload)
				{
					auto key = std::make_pair(fontName, fontSize);
					if (mFontResources.find(key) == mFontResources.end())
					{
						std::vector<uint8_t> binaryFileData = Utility::ReadPackedFile(virtualFilePath);
						LOG_DEBUG("Creating font resource: " + fontName +
							" size " + std::to_string(fontSize));
						mFontResources[key] = std::make_shared<FontResources>(mRenderResourceContext, this, fontName, binaryFileData, fontSize);
						mFontResources[key]->StoreFontAtlasesInFiles(bStoreFontAtlasesInFiles);
					}
					LOG_DEBUG("Preloading script '" + requestedScript +
						"' for font " + fontName +
						" size " + std::to_string(fontSize));
					const std::uint32_t rangeBegin = sScriptRanges[requestedScript].first;
					const std::uint32_t rangeEnd = sScriptRanges[requestedScript].second;
					mFontResources[key]->LoadGlyphsFromCodePointRange(rangeBegin, rangeEnd);
				}
			}
		}
	}

	auto end = std::chrono::steady_clock::now();
	float ms = std::chrono::duration<float, std::milli>(end - start).count();

	LOG_INFO("Font atlas preloading (package) completed in " +
		std::to_string(ms) + " ms.");
}

} //namespace rendering_engine