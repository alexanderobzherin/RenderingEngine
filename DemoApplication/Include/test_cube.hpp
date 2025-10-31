// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2025 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor.hpp"

#include <memory>

using namespace rendering_engine;

namespace rendering_engine
{
class Scene;
class Camera;

class StaticMesh;
}

namespace demo_application
{

class TestCube : public Actor
{
public:
	TestCube(Scene& scene);

    void Initialize() override;

    void Update(float deltaTime) override;

    void Draw(const Camera& camera) override;

private:
    std::unique_ptr<StaticMesh> mMesh;
};

}