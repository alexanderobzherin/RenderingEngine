#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include "boost/filesystem.hpp"

#include "utility.hpp"
#include "demo_application.hpp"


using namespace rendering_engine;
using namespace demo_application;

int main(int argc, char *argv[])
{
	std::cout << "Vulkan App is running..." << std::endl;
    Utility::InitializePaths(argc, argv);

    std::unique_ptr<DemoApplication> app = std::make_unique<DemoApplication>(800, 600, "Vulkan Application");
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
