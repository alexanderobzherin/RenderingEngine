#include "utility.hpp"

namespace rendering_engine
{
using namespace boost::filesystem;
path const Utility::sDefaultShadersBinaryRelativePath = {"/Intermediate/Shaders/"};
path const Utility::sTextureRelativePathFolder = path{} / "Intermediate" / "Textures";
path const Utility::sModelsRelativePathFolder =  path{} / "Intermediate" / "Models";
path const Utility::sShadersRelativePathFolder = path{} / "Intermediate" / "Shaders";;
path Utility::sShadersBinaryPath;
path Utility::sApplicationPath;
path Utility::sBuildPath;

void Utility::InitializePaths(int argc, char* argv[])
{
	sApplicationPath = boost::filesystem::path(argv[0]);

	sBuildPath = FindPath( "Build" );
	sShadersBinaryPath = sBuildPath += sDefaultShadersBinaryRelativePath;
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
		if( boost::filesystem::exists( boost::filesystem::path( directory ) ) && boost::filesystem::is_directory( boost::filesystem::path( directory ) ) )
		{
			boost::filesystem::path pathToDirectory = boost::filesystem::path( directory );

			if( boost::filesystem::is_directory( pathToDirectory ) )
			{
				for( boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator( pathToDirectory ) )
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
	catch( const boost::filesystem::filesystem_error& ex )
	{
		std::cout << ex.what() << '\n';
	}

	return shaderFileNames;
}

boost::filesystem::path Utility::GetApplicationPath()
{
	return sApplicationPath;
}

boost::filesystem::path Utility::GetBuildPath()
{
	return sBuildPath;
}

boost::filesystem::path Utility::GetShadersBinaryPath()
{
	return sShadersBinaryPath;
}


boost::filesystem::path Utility::FindPath(std::string fileOrFolderName, std::string searchingFrom)
{
	boost::filesystem::path result;
	for( boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(searchingFrom) )
	{
		if( x.path().filename().string() == fileOrFolderName )
		{
			result = x.path().generic_path();
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
		if( boost::filesystem::exists(boost::filesystem::path(directory)) && boost::filesystem::is_directory(boost::filesystem::path(directory)) )
		{
			boost::filesystem::path pathToDirectory = boost::filesystem::path(directory);

			if( boost::filesystem::is_directory(pathToDirectory) )
			{
				for( boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(pathToDirectory) )
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
	catch( const boost::filesystem::filesystem_error& ex )
	{
		std::cout << ex.what() << '\n';
	}

	return imageFileNames;
}

boost::filesystem::path Utility::ResolveProjectRoot()
{
	auto exeDir = boost::filesystem::canonical(boost::filesystem::path(boost::filesystem::current_path())); // default
	if (exeDir.filename() == "Debug" || exeDir.filename() == "Release")
		exeDir = exeDir.parent_path(); // step out of Debug/Release
	if (exeDir.filename() == "Binaries")
		exeDir = exeDir.parent_path(); // step out of Binaries
	return exeDir;
}

boost::filesystem::path Utility::GetTextureFolderPath()
{
	return ResolveProjectRoot() / sTextureRelativePathFolder;
}

boost::filesystem::path Utility::GetModelsFolderPath()
{
	return ResolveProjectRoot() / sModelsRelativePathFolder;
}

boost::filesystem::path Utility::GetShadersFolderPath()
{
	return ResolveProjectRoot() / sShadersRelativePathFolder;
}

}