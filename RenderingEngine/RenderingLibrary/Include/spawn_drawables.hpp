// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

/**
 * @file spawn_drawables.hpp
 * @brief Scene::Spawn<T> specializations for built-in drawable types.
 *
 * This file provides explicit template specializations of Scene::Spawn<T>()
 * for engine-supported drawable classes (e.g. StaticMesh, Sprite2D, TextBlock2D).
 *
 * Including this file enables spawning of the corresponding drawable types.
 * Scene itself remains generic and unaware of concrete drawable implementations.
 */

#pragma once

#include "scene.hpp"
#include "scene_manager.hpp"

#include "static_mesh.hpp"
#include "sprite_2d.hpp"
#include "text_block_2d.hpp"
#include "rectangle_2d.hpp"

namespace rendering_engine
{
	
template <>
StaticMesh* Scene::Spawn<StaticMesh, StaticMeshParams>(StaticMeshParams param);

template <>
Sprite2D* Scene::Spawn<Sprite2D, std::string>(std::string textureName);

template <>
TextBlock2D* Scene::Spawn<TextBlock2D, TextBlock2D::Properties>(TextBlock2D::Properties prop);
	

template <>
StaticMesh* Scene::Spawn<StaticMesh, StaticMeshParams>(StaticMeshParams param)
{
	mDrawables3D.push_back(new StaticMesh(mSceneManager.GetRenderResourceContext(), *this, param));
	StaticMesh* staticMesh = static_cast<StaticMesh*>(mDrawables3D.back());
	staticMesh->Initialize();
	return staticMesh;
}

template <>
Sprite2D* Scene::Spawn<Sprite2D, std::string>(std::string textureName)
{
	mDrawables2D.push_back(new Sprite2D(mSceneManager.GetRenderResourceContext(), *this, textureName));
	Sprite2D* sprite2D = static_cast<Sprite2D*>(mDrawables2D.back());
	sprite2D->Initialize();
	return sprite2D;
}
template <>
TextBlock2D* Scene::Spawn<TextBlock2D, TextBlock2D::Properties>(TextBlock2D::Properties prop)
{
	mDrawables2D.push_back(new TextBlock2D(*this, mSceneManager.GetTextRenderer(), prop));
	TextBlock2D* textBlock = static_cast<TextBlock2D*>(mDrawables2D.back());
	return textBlock;
}

template <>
Rectangle2D* Scene::Spawn<Rectangle2D, Rectangle2D::Properties>(Rectangle2D::Properties prop)
{
	mDrawables2D.push_back(new Rectangle2D(mSceneManager.GetRenderResourceContext(), *this, prop));
	Rectangle2D* rectangle2D = static_cast<Rectangle2D*>(mDrawables2D.back());
	rectangle2D->Initialize();
	return rectangle2D;
}

} // namespace rendering_engine