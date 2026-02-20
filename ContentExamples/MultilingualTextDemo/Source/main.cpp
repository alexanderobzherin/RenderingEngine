// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include <cstdint>
#include <memory>
#include <stdexcept>

#include "level_register.hpp"
#include "utility.hpp"
#include "core_application.hpp"


using namespace rendering_engine;

int main(int argc, char *argv[])
{
    Utility::InitializePaths(argc, argv);

    std::unique_ptr<CoreApplication> app = std::make_unique<CoreApplication>();
    try
    {
        app->Initialize();
        app->Run();
    }
    catch( std::exception const& exc )
    {
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
