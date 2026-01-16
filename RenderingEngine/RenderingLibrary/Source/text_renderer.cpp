#include "text_renderer.hpp"

#include "render_resource_context.hpp"

#include "font_resources.hpp"
#include "utility.hpp"

namespace rendering_engine
{
/*
Latin       0x0020 – 0x007E
Cyrillic    0x0400 – 0x04FF
Hebrew      0x0590 – 0x05FF
Arabic      0x0600 – 0x06FF
*/
std::unordered_map<std::string, std::pair<std::uint32_t, std::uint32_t>> TextRenderer::sScriptRanges{
	{{"latin"}, {0x0020, 0x007E}},
	{{"cyrillic"}, {0x0400, 0x04FF}},
	{{"arabic"}, {0x0600, 0x08FF}}
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

		for (const auto& script : appConfig.textScripts)
		{
			if (sScriptRanges.find(script) == sScriptRanges.end())
				continue;

			const std::uint32_t rangeBegin = sScriptRanges[script].first;
			const std::uint32_t rangeEnd = sScriptRanges[script].second;
			mFontResources[fontName]->LoadGlyphsFromRange(rangeBegin, rangeEnd);
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

} //namespace rendering_engine