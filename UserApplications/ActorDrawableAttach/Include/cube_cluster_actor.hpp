// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor.hpp"

using namespace rendering_engine;

namespace rendering_engine
{
class StaticMesh;
}

namespace application
{

class CubeClusterActor : public Actor
{
public:
	CubeClusterActor(Scene& scene);

	void Initialize() override;

	void Update(float deltaTime) override;

private:
	const float mAngularVelocity = 1.0f;

	StaticMesh* mCenterCubeMesh;
	StaticMesh* mCubeMesh_1;
	StaticMesh* mCubeMesh_2;
	StaticMesh* mCubeMesh_3;
	StaticMesh* mCubeMesh_4;
	StaticMesh* mCubeMesh_5;
	StaticMesh* mCubeMesh_6;

	StaticMesh* mCubeMesh_5_1;
	StaticMesh* mCubeMesh_6_1;

	const float mSpeed = 0.05f;
	glm::vec3 mMovementDirection = glm::vec3(0.0f, 1.0f, 0.0f);
};

} //application