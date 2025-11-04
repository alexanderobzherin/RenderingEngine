#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "level_register.hpp"
#include "utility.hpp"
#include "core_application.hpp"


using namespace rendering_engine;

int main(int argc, char *argv[])
{
	std::cout << "Demo App is running..." << std::endl;
    Utility::InitializePaths(argc, argv);

    std::unique_ptr<CoreApplication> app = std::make_unique<CoreApplication>(800, 600, "Demo Application");
    try
    {
        app->Initialize();
        app->Run();
        app->Shutdown();
    }
    catch( std::exception const& exc )
    {
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
