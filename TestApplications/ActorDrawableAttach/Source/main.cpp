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

/**
 * ============================================================================
 * Actor & SceneComponent Hierarchy Test Application
 * ============================================================================
 *
 * This test application validates the core actor and transformation system
 * of the Rendering Engine.
 *
 * The purpose of this test is to verify:
 *
 * 1. Actor / Actor2D lifecycle management
 *    - Spawning actors via Scene
 *    - Deferred destruction through Scene queues
 *    - Proper Shutdown() execution before deletion
 *
 * 2. SceneComponent hierarchical transformations
 *    - Local transform (position, rotation, scale)
 *    - Parent-child attachment
 *    - Correct world matrix propagation
 *    - Nested attachment chains
 *
 * 3. Drawable attachment to actors
 *    - StaticMesh attached to Actor (3D)
 *    - Rectangle2D and TextBlock2D attached to Actor2D (2D)
 *    - Transform inheritance from root component
 *
 * 4. Runtime transform updates
 *    - Continuous rotation updates
 *    - Continuous translation updates
 *    - Direction reversal at bounds
 *    - Correct propagation of dynamic changes
 *
 * 5. Spawn / Destroy stress testing
 *    - Actors destroyed and respawned periodically
 *    - No dangling parent pointers
 *    - No double deletion
 *    - No validation layer errors
 *
 * Scene Content:
 * --------------
 * - CubeClusterActor (3D):
 *     A central cube with multiple child cubes attached in hierarchy.
 *     Demonstrates nested SceneComponent attachment and world transform
 *     propagation under rotation and translation.
 *
 * - TestActor2D (2D):
 *     A rotating and translating rectangle with attached TextBlock2D elements.
 *     Demonstrates 2D hierarchical transform behavior and synchronized updates.
 *
 * This test ensures stability of:
 *  - Actor architecture
 *  - Transform propagation
 *  - Scene-managed memory lifecycle
 *  - Drawable attachment model
 *
 * This application serves as a foundational validation before expanding
 * the engine with further systems (collision, input, UI, gameplay logic).
 * ============================================================================
 */