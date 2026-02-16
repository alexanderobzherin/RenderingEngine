#include "text_renderer.hpp"
#include "render_resource_context.hpp"
#include "font_resources.hpp"
#include "logger.hpp"

namespace rendering_engine
{
std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> TextRenderer::sScriptRanges{
	// European
	{{"Latin"}, {0x0020, 0x007E}},
	{{"Cyrillic"}, {0x0400, 0x04FF}},
	{{"Greek"}, {0x0370, 0x03FF}},

	// Asian
	{{"Han"}, {0x4E00, 0x9FFF}},
	{{"HanExtensionA"}, {0x3400, 0x4DBF}},

	{{"Hiragana"}, {0x3040, 0x309F}},
	{{"Katakana"}, {0x30A0, 0x30FF}},
	{{"KatakanaPhoneticExtensions"}, {0x31F0, 0x31FF}},

	{{"Hangul"}, {0xAC00, 0xD7AF}},

	// Requires shaping
	// East
	{{"Hebrew"}, {0x0590, 0x05FF}},
	{{"Arabic"}, {0x0600, 0x06FF}},
	{{"ArabicSupplement"}, {0x0750, 0x077F}},
	{{"ArabicExtended-A"}, {0x08A0, 0x08FF}},
	{{"Aramaic"}, {0x0700, 0x074F}},
	{{"Thaana"}, {0x0780, 0x07BF}}, 

	// Indic
	{{"Devanagari"}, {0x0900, 0x097F}},
	{{"Bengali"}, {0x0980, 0x09FF}},	
	{{"Gurmukhi"}, {0x0A00, 0x0A7F}},	
	{{"Gujarati"}, {0x0A80, 0x0AFF}},	
	{{"Oriya"}, {0x0B00, 0x0B7F}},		
	{{"Tamil"}, {0x0B80, 0x0BFF}},		
	{{"Telugu"}, {0x0C00, 0x0C7F}},		
	{{"Kannada"}, {0x0C80, 0x0CFF}},	
	{{"Malayalam"}, {0x0D00, 0x0D7F}},	
	{{"Sinhala"}, {0x0D80, 0x0DFF}},	

	// Southeast Asian
	{{"Thai"}, {0x0E00, 0x0E7F}},		
	{{"Lao"}, {0x0E80, 0x0EFF}},		
	{{"Myanmar"}, {0x1000, 0x109F}},	
	{{"Khmer"}, {0x1780, 0x17FF}},		

	// Tibetan
	{{"Tibetan"}, {0x0F00, 0x0FFF}}		
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

std::shared_ptr<FontResources> TextRenderer::GetFontResources(const std::string& fontName, int fontSize)
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
	std::pair<std::uint32_t, std::uint32_t> result{0, 0};

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
	boost::filesystem::path pathToDirectory = boost::filesystem::path(pathToFolder);
	const bool isValidFolderPath = boost::filesystem::exists(boost::filesystem::path(pathToFolder)) && boost::filesystem::is_directory(boost::filesystem::path(pathToFolder));
	if (!isValidFolderPath)
	{
		return;
	}
	// 2. Iterate through files in the folder.
	//    if file is in the list of supported extensions
	for (boost::filesystem::recursive_directory_iterator it(pathToDirectory), end;
		it != end;
		++it)
	{
		const boost::filesystem::path& filePath = it->path();

		if (!boost::filesystem::is_regular_file(filePath))
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
			auto fontFilePath = boost::filesystem::path(virtualPath);
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