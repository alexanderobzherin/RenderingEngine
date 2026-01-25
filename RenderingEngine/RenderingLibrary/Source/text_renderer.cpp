#include "text_renderer.hpp"

#include "render_resource_context.hpp"

#include "font_resources.hpp"
#include "utility.hpp"

namespace rendering_engine
{
std::vector<std::uint32_t> sFontSizesPreload{10, 14, 16};
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
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::LoadFontsFromFolder(std::string pathToFolder)
{
	// Load configured languages.

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

		std::string fontName = filePath.stem().string();

		const unsigned int fontSize = 16;
		mFontResources[fontName] = std::make_shared<FontResources>(mRenderResourceContext, filePath.string(), fontSize);
		mFontResources[fontName]->StoreFontAtlasesInFiles(bStoreFontAtlasesInFiles);

		AppConfig appConfig = Utility::ReadConfigFile();
		bTextShapingEnabled = appConfig.testShapingEnabled;

		for (const auto& script : appConfig.textScripts)
		{
			if (sScriptRanges.find(script) == sScriptRanges.end())
				continue;

			const std::uint32_t rangeBegin = sScriptRanges[script].first;
			const std::uint32_t rangeEnd = sScriptRanges[script].second;
			mFontResources[fontName]->LoadGlyphsFromCodePointRange(rangeBegin, rangeEnd);
		}
	}
}

void TextRenderer::LoadFontsFromPackage()
{
}

const RenderResourceContext& TextRenderer::GetRenderResourceContext() const
{
	return mRenderResourceContext;
}

std::shared_ptr<FontResources> TextRenderer::GetFontResources(std::string fontName)
{
	if (auto search = mFontResources.find(fontName); search != mFontResources.end())
	{
		return search->second;
	}

	return nullptr;
}

void TextRenderer::StoreFontAtlasesInFiles(bool in)
{
	bStoreFontAtlasesInFiles = in;
}

std::vector<std::string> TextRenderer::GetScriptsRequiredShaping()
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

} //namespace rendering_engine