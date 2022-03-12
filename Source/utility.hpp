#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"

namespace rendering_engine
{
class Utility final
{
    public:
	static void InitializePaths(int argc, char* argv[]);
    static std::vector<char> ReadShaderBinaryFile( std::string const & filename );
    static std::vector<std::string> GetListOfFilesInDirectory( std::string directory );
	static boost::filesystem::path GetApplicationPath();
	static boost::filesystem::path GetBuildPath();
	static boost::filesystem::path GetShadersBinaryPath();

	private:
		Utility();
		Utility( const Utility& rhs );
		Utility& operator=( const Utility& rhs );

		static boost::filesystem::path FindPath( std::string fileOrFolderName, std::string searchingFrom = "../../" );

	private:
		static boost::filesystem::path sApplicationPath;
		static boost::filesystem::path const sDefaultShadersBinaryRelativePath;
		static boost::filesystem::path sBuildPath;
		static boost::filesystem::path sShadersBinaryPath;
		
};
}