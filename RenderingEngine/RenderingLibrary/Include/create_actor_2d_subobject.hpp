// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"
#include "scene.hpp"

#include "rectangle_2d.hpp"
#include "text_block_2d.hpp"

namespace rendering_engine
{
template <>
Rectangle2D* Actor2D::CreateSubobject<Rectangle2D>(Rectangle2D::Properties prop)
{
	auto rectangle2D = mScene.Spawn<Rectangle2D>(prop);
	mWards.push_back(rectangle2D);
	rectangle2D->UpdateOnTick(bUpdateOnTick);
	
	return rectangle2D;
}
	
template <>
TextBlock2D* Actor2D::CreateSubobject<TextBlock2D>(TextBlock2D::Properties prop)
{
	auto textBlock2D = mScene.Spawn<TextBlock2D>(prop);
	mWards.push_back(textBlock2D);
	textBlock2D->UpdateOnTick(bUpdateOnTick);
	
	return textBlock2D;
}
} // namespace rendering_engine
