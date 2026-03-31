// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <string>
#include <vector>

namespace test_app
{
struct TextureLoadMetrics
{
    std::string file;
    double loadTimeMs = 0.0;
    double copyTimeMs = 0.0;
    unsigned int width = 0;
    unsigned int height = 0;
    size_t sizeBytes = 0;
};

struct TextureLoadSummary
{
    std::string file;
    unsigned int width = 0;
    unsigned int height = 0;
    size_t sizeBytes = 0;

    double loadAvgMs = 0.0;
    double loadMinMs = 0.0;
    double loadMaxMs = 0.0;

    double copyAvgMs = 0.0;
    double copyMinMs = 0.0;
    double copyMaxMs = 0.0;
};

class Benchmark
{
public:
	Benchmark();

	void BenchmarkTextureLoadAndCopy(size_t numOfIterations = 0);
private:
    TextureLoadMetrics LoadTextureFile(std::string const& texturePath);
    TextureLoadSummary ComputeTextureSummary(const std::vector<TextureLoadMetrics>& metrics);
    void PrintTextureTestSummary(const std::vector<TextureLoadSummary>& summary);

    static constexpr size_t DefaultNumOfIterations = 20;
};
}