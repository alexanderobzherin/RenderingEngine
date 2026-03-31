// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <iostream>

#include "level_register.hpp"
#include "utility.hpp"
#include "core_application.hpp"

#include "benchmark.hpp"

using namespace test_app;
using namespace rendering_engine;

/**
 * @brief Texture loading and copy benchmark entry point.
 *
 * This test application measures CPU-side texture loading performance of the
 * ImageData class. For each supported texture file found in the application's
 * Content/Textures folder, the benchmark performs:
 *
 * 1. One warm-up load (not included in summary results)
 * 2. Repeated measured texture loads
 * 3. Repeated measured ImageData copy operations
 *
 * The benchmark prints per-iteration timings and a final summary containing:
 * - average load time
 * - minimum load time
 * - maximum load time
 * - average copy time
 * - minimum copy time
 * - maximum copy time
 *
 * Usage notes:
 * - Run in Release build mode when evaluating performance.
 *   Debug build timings are not representative.
 * - The default number of measured iterations is defined in Benchmark as
 *   DefaultNumOfIterations.
 * - If needed, the number of iterations can be changed either in code or by
 *   passing an explicit value to BenchmarkTextureLoadAndCopy().
 * - Place benchmark textures into the application's Content/Textures folder.
 * - Use this tool to compare before/after timings when optimizing ImageData
 *   implementation or introducing multithreaded texture processing.
 */

int main(int argc, char *argv[])
{
    Utility::InitializePaths(argc, argv);

    try
    {
        std::unique_ptr<Benchmark> benchmark = std::make_unique<Benchmark>();
        benchmark->BenchmarkTextureLoadAndCopy();
    }
    catch( std::exception const& exc )
    {
        std::cerr << "Benchmark failed: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }

	return EXIT_SUCCESS;
}
