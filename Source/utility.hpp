#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

namespace rendering_engine
{
class Utility
{
    public:
    static std::vector<char> ReadShaderBinaryFile( std::string const & filename );
    static std::vector<std::string> GetListOfFilesInDirectory( std::string directory );

	private:
		Utility();
		Utility( const Utility& rhs );
		Utility& operator=( const Utility& rhs );

	public:
		static std::string const sDefaultShadersBinaryPath;
		static std::string sApplicationPath;

};
}