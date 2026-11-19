// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"
#include "scene.hpp"

#include "rectangle_2d.hpp"
#include "text_block_2d.hpp"
#include "sprite_2d.hpp"

namespace rendering_engine
{
template <>
inline Rectangle2D* Actor2D::CreateSubobject<Rectangle2D>(Rectangle2D::Properties prop)
{
	auto rectangle2D = mScene.Spawn<Rectangle2D>(prop);
	mWards.push_back(rectangle2D);
	rectangle2D->UpdateOnTick(bUpdateOnTick);
	
	return rectangle2D;
}
	
template <>
inline TextBlock2D* Actor2D::CreateSubobject<TextBlock2D>(TextBlock2D::Properties prop)
{
	auto textBlock2D = mScene.Spawn<TextBlock2D>(prop);
	mWards.push_back(textBlock2D);
	textBlock2D->UpdateOnTick(bUpdateOnTick);
	
	return textBlock2D;
}

template <>
inline Sprite2D* Actor2D::CreateSubobject<Sprite2D, std::string>(std::string textureName)
{
	auto sprite2D = mScene.Spawn<Sprite2D>(textureName);
	mWards.push_back(sprite2D);
	sprite2D->UpdateOnTick(bUpdateOnTick);

	return sprite2D;
}
} // namespace rendering_engine
