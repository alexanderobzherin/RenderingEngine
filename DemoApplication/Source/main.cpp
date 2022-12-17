#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include "boost/filesystem.hpp"
#include "vulkan_renderer.hpp"
#include "utility.hpp"

using namespace rendering_engine;
using namespace vulkan_renderer;

int main(int argc, char *argv[])
{
	std::cout << "Vulkan App is running..." << std::endl;
    Utility::InitializePaths(argc, argv);

    std::unique_ptr<VulkanRenderer> app = std::make_unique<VulkanRenderer>(800, 600, "Vulkan Application");
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
