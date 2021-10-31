#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "rendering_application.hpp"

using namespace rendering_engine;

int main()
{
	std::cout << "Vulkan App is running..." << std::endl;

    std::unique_ptr<RenderingApplication> app = std::make_unique<RenderingApplication>( 800, 600, "Vulkan Application" );
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
