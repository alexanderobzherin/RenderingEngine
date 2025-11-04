#include "version.h"

#include <iostream>
#include <cstdint>
#include <memory>
#include <stdexcept>

void PrintEngineVersion()
{
    std::cout << "Rendering Engine v"
              << RENDERING_ENGINE_VERSION_STRING
              << " (" << RENDERING_ENGINE_BUILD_PLATFORM << ", "
              << RENDERING_ENGINE_BUILD_DATE << ")"
              << std::endl;
}

int main(int argc, char *argv[])
{
	std::cout << "Material Compiler is running..." << std::endl;

    PrintEngineVersion();

	return EXIT_SUCCESS;
}
