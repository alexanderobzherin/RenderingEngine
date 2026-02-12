// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"

using namespace rendering_engine;

namespace rendering_engine
{
class Rectangle2D;
class TextBlock2D;
}

namespace application
{

class TestActor2D : public Actor2D
{
public:
	TestActor2D(Scene& scene);

	void Initialize() override;

	void Update(float deltaTime) override;

private:
	float mAngularVelocity = 0.3f;

	Rectangle2D* mRectangle2D;
	TextBlock2D* mLeftTextBlock2D;
	TextBlock2D* mRightTextBlock2D;
	
	const float mSpeed = 1.0f;
	glm::vec2 mMovementDirection = glm::vec2(1.0f, 0.0f);
};

} // namespace rendering_engine