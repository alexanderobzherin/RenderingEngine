// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"

using namespace rendering_engine;

namespace rendering_engine
{
class TextBlock2D;
class Rectangle2D;
}

namespace application
{

class TestButton : public Actor2D
{
public:
	TestButton(Scene& scene);

	void Initialize() override;

	void Update(float deltaTime) override;

	void OnButtonPressed();
	void OnButtonReleased();

private:
	void SetButtonShape();
	void SetButtonLabel();

private:
	static const glm::vec3 sRectInactiveColor;
	static const glm::vec3 sRectActiveColor;
	static const glm::vec2 sBaseButtonSize;

	static const glm::vec3 sTextInactiveColor;
	static const glm::vec3 sTextActiveColor;
	static const std::string sLabelFontName;
	static const unsigned int sLabelFontSize;

	TextBlock2D* mTextBlock2D;
	Rectangle2D* mRectangle2D;

	glm::vec2 mButtonSize{5.0f, 2.0f};
	std::string mButtonLabel{"Test Button"};
};

} // namespace application