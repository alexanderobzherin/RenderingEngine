#include "utility.hpp"
#include <nlohmann/json.hpp>

namespace rendering_engine
{
using namespace std::filesystem;

path const Utility::sDefaultShadersBinaryRelativePath = {"/Content/Shaders/"};
path const Utility::sContentRelativePathFolder = path{} / "Content";
path const Utility::sTextureRelativePathFolder = sContentRelativePathFolder / "Textures";
path const Utility::sModelsRelativePathFolder = sContentRelativePathFolder / "Models";
path const Utility::sFontsRelativePathFolder = sContentRelativePathFolder / "Fonts";
path const Utility::sShadersRelativePathFolder = sContentRelativePathFolder / "Shaders";
path const Utility:: sAppConfigFilePath = path{} / "Config" / "app_config.json";
path const Utility::sLogFolderPath = path{} / "Logs";

path Utility::sShadersBinaryPath;
path Utility::sApplicationPath;
path Utility::sBuildPath;

static PackEntries sPackEntries;
static bool sPackEntriesLoaded = false;
path const Utility::sContentPackageFilePath = path{} / "Content" / "Pack.bin";
path const Utility::sContentPackEntriesFilePath = path{} / "Content" / "Pack.json";

void Utility::InitializePaths(int argc, char* argv[])
{
	sApplicationPath = std::filesystem::path(argv[0]);

	sBuildPath = FindPath( "Build" );
	sShadersBinaryPath = sBuildPath += sDefaultShadersBinaryRelativePath;
}

AppConfig Utility::ReadConfigFile()
{
	AppConfig cfg;

	std::ifstream f(GetConfigFilePath().string());
	if (!f.is_open())
	{
		return cfg;
	}

	try
	{
		nlohmann::json appConfigData = nlohmann::json::parse(f);

		if (appConfigData.contains("appName"))
			cfg.appName = appConfigData["appName"].get<std::string>();

		if (appConfigData.contains("isFullScreen"))
			cfg.isFullScreen = appConfigData["isFullScreen"].get<bool>();

		if (appConfigData.contains("screenWidth"))
			cfg.screenWidth = appConfigData["screenWidth"].get<float>();

		if (appConfigData.contains("screenHeight"))
			cfg.screenHeight = appConfigData["screenHeight"].get<float>();

		if (appConfigData.contains("text"))
		{
			const auto& textNode = appConfigData["text"];

			if (textNode.contains("scripts") && textNode["scripts"].is_array())
			{
				for (const auto& script : textNode["scripts"])
				{
					if (script.is_string())
					{
						auto found = std::find(cfg.textScripts.begin(), cfg.textScripts.end(), script);
						if(found == cfg.textScripts.end())
						{
							cfg.textScripts.push_back(script.get<std::string>());
						}
					}
				}
			}

			if (textNode.contains("fontSizePreload") && textNode["fontSizePreload"].is_array())
			{
				for (const auto& fontSize : textNode["fontSizePreload"])
				{
					auto found = std::find(cfg.fontSizePreload.begin(), cfg.fontSizePreload.end(), fontSize);
					if (found == cfg.fontSizePreload.end())
					{
						cfg.fontSizePreload.push_back(fontSize.get<int>());
					}
				}
			}
		}

		if (appConfigData.contains("logLevel"))
			cfg.logLevel = appConfigData["logLevel"].get<std::string>();

		if (appConfigData.contains("useSmoothedFPS"))
			cfg.useSmoothedFPS = appConfigData["useSmoothedFPS"].get<bool>();

		if (appConfigData.contains("targetFPS"))
			cfg.targetFPS = appConfigData["targetFPS"].get<float>();

		if (appConfigData.contains("showStatsOverlay"))
			cfg.showStatsOverlay = appConfigData["showStatsOverlay"].get<bool>();
		
	}
	catch (const std::exception& e)
	{
		return cfg;
	}

	return cfg;
}

std::vector<char> Utility::ReadShaderBinaryFile( std::string const & filename )
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) 
    {
        throw std::runtime_error("failed to open shader binary file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

std::vector<std::string> Utility::GetListOfFilesInDirectory( std::string directory )
{
    std::vector<std::string> shaderFileNames;

	try
	{
		//check if parameter string is directory
		if( std::filesystem::exists( std::filesystem::path( directory ) ) && std::filesystem::is_directory(std::filesystem::path( directory ) ) )
		{
			std::filesystem::path pathToDirectory = std::filesystem::path( directory );

			if(std::filesystem::is_directory( pathToDirectory ) )
			{
				for(const std::filesystem::directory_entry& x : std::filesystem::directory_iterator( pathToDirectory ) )
				{
					size_t dot = x.path().string().find_last_of( "." );

					if( std::string{ "spv" } == x.path().string().substr( dot + 1 ) )
					{
						std::cout << "Shader binary file: " << x.path().string() << "\n";
						shaderFileNames.push_back( x.path().string() );
					}
				}
			}
		}
	}
	catch( const std::filesystem::filesystem_error& ex )
	{
		std::cout << ex.what() << '\n';
	}

	return shaderFileNames;
}

std::filesystem::path Utility::GetApplicationPath()
{
	return sApplicationPath;
}

std::filesystem::path Utility::GetBuildPath()
{
	return sBuildPath;
}

std::filesystem::path Utility::GetShadersBinaryPath()
{
	return sShadersBinaryPath;
}


std::filesystem::path Utility::FindPath(std::string fileOrFolderName, std::string searchingFrom)
{
	std::filesystem::path result;
	for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(searchingFrom) )
	{
		if( entry.path().filename().string() == fileOrFolderName )
		{
			result = entry.path();
			break;
		}
	}
	return result;
}

std::vector<std::string> Utility::GetListOfFileNamesInDirectory(const char* directory, std::string extToSearch)
{
	std::vector<std::string> imageFileNames;

	try
	{
		//check if parameter string is directory
		if(std::filesystem::exists(std::filesystem::path(directory)) && std::filesystem::is_directory(std::filesystem::path(directory)) )
		{
			std::filesystem::path pathToDirectory = std::filesystem::path(directory);

			if(std::filesystem::is_directory(pathToDirectory) )
			{
				for(const std::filesystem::directory_entry& x : std::filesystem::directory_iterator(pathToDirectory) )
				{
					size_t dot = x.path().string().find_last_of(".");

					if( extToSearch == x.path().string().substr(dot + 1) )
					{
						imageFileNames.push_back(x.path().string());
					}
				}
			}
		}
	}
	catch( const std::filesystem::filesystem_error& ex )
	{
		std::cout << ex.what() << '\n';
	}

	return imageFileNames;
}

std::filesystem::path Utility::ResolveProjectRoot()
{
	auto exeDir = std::filesystem::canonical(std::filesystem::path(std::filesystem::current_path())); // default
	if (exeDir.filename() == "Debug" || exeDir.filename() == "Release")
		exeDir = exeDir.parent_path(); // step out of Debug/Release
	if (exeDir.filename() == "Binaries")
		exeDir = exeDir.parent_path(); // step out of Binaries
	return exeDir;
}

std::filesystem::path Utility::GetContentFolderPath()
{
	return ResolveProjectRoot() / sContentRelativePathFolder;
}

std::filesystem::path Utility::GetTextureFolderPath()
{
	return ResolveProjectRoot() / sTextureRelativePathFolder;
}

std::filesystem::path Utility::GetModelsFolderPath()
{
	return ResolveProjectRoot() / sModelsRelativePathFolder;
}

std::filesystem::path Utility::GetFontsFolderPath()
{
	return ResolveProjectRoot() / sFontsRelativePathFolder;
}

std::filesystem::path Utility::GetShadersFolderPath()
{
	return ResolveProjectRoot() / sShadersRelativePathFolder;
}

std::filesystem::path Utility::GetConfigFilePath()
{
	return ResolveProjectRoot() / sAppConfigFilePath;
}

std::filesystem::path Utility::GetLogsFolderPath()
{
	return ResolveProjectRoot() / sLogFolderPath;
}

bool Utility::IsPackageProvided()
{
	const auto root = ResolveProjectRoot();
	return std::filesystem::exists(root / sContentPackageFilePath) &&
		std::filesystem::exists(root / sContentPackEntriesFilePath);
}

const PackEntries& Utility::GetPackEntries()
{
	if (sPackEntriesLoaded)
		return sPackEntries;

	sPackEntries.clear();

	// Path to Pack.json
	const std::filesystem::path jsonPath = ResolveProjectRoot() / sContentPackEntriesFilePath;

	if (!std::filesystem::exists(jsonPath))
	{
		sPackEntriesLoaded = true;
		return sPackEntries; // empty
	}

	// Load JSON
	std::ifstream f(jsonPath.string());
	if (!f.is_open())
	{
		std::cerr << "[ERROR] Failed to open Pack.json\n";
		sPackEntriesLoaded = true;
		return sPackEntries;
	}

	nlohmann::json j;
	f >> j;

	// Parse entries
	for (auto it = j.begin(); it != j.end(); ++it)
	{
		PackEntry entry;
		entry.offset = it.value().value("offset", 0);
		entry.size = it.value().value("size", 0);
		sPackEntries[it.key()] = entry;
	}

	sPackEntriesLoaded = true;
	return sPackEntries;
}

std::vector<uint8_t> Utility::ReadPackedFile(const std::string& entryPath)
{
	std::vector<std::uint8_t> data;

	if (!IsPackageProvided())
		return data;

	const path binPath = ResolveProjectRoot() / sContentPackageFilePath;
	const path jsonPath = ResolveProjectRoot() / sContentPackEntriesFilePath;

	if (!std::filesystem::exists(binPath) ||
		!std::filesystem::exists(jsonPath))
	{
		std::cerr << "[Utility::ReadPackedFile] Missing Pack.bin or Pack.json\n";
		return data;
	}

	GetPackEntries();
	// Check if this entry exists in Pack.json
	auto it = sPackEntries.find(entryPath);
	if (it == sPackEntries.end())
	{
		std::cerr << "[Utility::ReadPackedFile] No such packed entry: "
			<< entryPath << std::endl;
		return data; // empty
	}

	const PackEntry& entry = it->second;

	std::ifstream bin(binPath.string(), std::ios::binary);
	if (!bin)
	{
		std::cerr << "[Utility::ReadPackedFile] Failed to open Pack.bin: "
			<< binPath.string() << std::endl;
		return data;
	}

	// Read the memory region [offset, offset + size)

	data.resize(entry.size);

	bin.seekg(entry.offset, std::ios::beg);
	if (!bin.good())
	{
		std::cerr << "[Utility::ReadPackedFile] Seek error for entry: "
			<< entryPath << std::endl;
		return {};
	}

	bin.read(reinterpret_cast<char*>(data.data()), entry.size);
	if (!bin.good())
	{
		std::cerr << "[Utility::ReadPackedFile] Read error for entry: "
			<< entryPath << std::endl;
		return {};
	}

	return data;
}


}