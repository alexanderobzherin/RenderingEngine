// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor.hpp"
#include "scene.hpp"

#include "static_mesh.hpp"

namespace rendering_engine
{

template <>
StaticMesh* Actor::CreateSubobject<StaticMesh>(StaticMeshParams params)
{
	auto staticMesh = mScene.Spawn<StaticMesh>(params);
	mWards.push_back(staticMesh);
	staticMesh->UpdateOnTick(bUpdateOnTick);

	return staticMesh;
}


} // namespace rendering_engine
