#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include "boost/filesystem.hpp"
#include "rendering_application.hpp"
#include "utility.hpp"

using namespace rendering_engine;

int main(int argc, char *argv[])
{
	std::cout << "Vulkan App is running..." << std::endl;
    Utility::InitializePaths(argc, argv);
    std::unique_ptr<RenderingApplication> app = std::make_unique<RenderingApplication>( /*800, 600,*/ "Vulkan Application" );
    try
    {
        app->Run();
    }
    catch( std::exception const& exc )
    {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
