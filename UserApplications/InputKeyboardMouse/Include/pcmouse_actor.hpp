// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"

using namespace rendering_engine;

namespace rendering_engine
{
	class Sprite2D;
	class TextBlock2D;
}

namespace application
{

class PCMouseActor : public Actor2D
{
public:
	PCMouseActor(Scene& scene);

	void Initialize() override;

	void Update(float deltaTime) override;

	void OnLeftButtonPressed();
	void OnLeftButtonReleased();

	void OnRightButtonPressed();
	void OnRightButtonReleased();

	// Will need extension to identify when wheel scrolling ended
	void OnWheelUp();
	void OnWheelDown();

private:
	static const glm::vec3 sInactiveColor;
	static const glm::vec3 sActiveColor;

	Sprite2D* mMouseBottom;
	Sprite2D* mMouseLB;
	Sprite2D* mMouseRB;
	Sprite2D* mMouseWheelDown;
	Sprite2D* mMouseWheelUp;

	TextBlock2D* mCursorPosTextLabel;
	TextBlock2D* mCursorPosCoordinates;

	float mWheelUpHighlightTime = 0.0f;
	float mWheelDownHighlightTime = 0.0f;
};

} // namespace application