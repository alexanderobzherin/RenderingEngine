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

/*
    Dynamic Spawn / Destroy Test Application
    ----------------------------------------

    This application is designed to validate the Scene-level lifecycle
    management of drawable objects introduced in RE-100.

    The test continuously and randomly spawns and destroys different
    types of drawables (3D meshes, 2D sprites, and text blocks) using
    the public Scene::Spawn<T>() and Drawable::Destroy() interfaces.

    Goals of this test:
    - Verify that all drawables are created exclusively via Scene::Spawn<T>()
    - Ensure that destruction is routed through Scene-level deferred cleanup
    - Validate that render resources are released safely and correctly
    - Detect potential issues with container mutation, dangling pointers,
      or invalid lifetime assumptions
    - Confirm stability under repeated spawn/destroy cycles over time

    The application runs indefinitely and is intended to be observed
    during execution (visually and via diagnostics) to ensure there are:
    - no crashes
    - no resource leaks
    - no undefined behavior during update or render passes

    This test does not measure performance; it focuses solely on correctness
    and robustness of the Spawn/Destroy mechanism.
*/