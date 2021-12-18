#include "utility.hpp"
#include "boost/filesystem.hpp"

namespace rendering_engine
{
std::vector<char> Utility::ReadShaderBinaryFile( std::string const & filename )
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) 
    {
        throw std::runtime_error("failed to open file!");
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
					//std::string lookingExtension{ "jpg" };
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
}