// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"
#include "key_definition.hpp"

using namespace rendering_engine;

namespace rendering_engine
{
	class Rectangle2D;
	class TextBlock2D;
	enum class Key;
}

namespace application
{

class PCKeyboardKeyActor : public Actor2D
{
public:
	PCKeyboardKeyActor(Scene& scene);

	void Initialize() override;
	void Update(float deltaTime) override;

	void OnKeyPressed();
	void OnKeyReleased();

	void ConfigureKey(const KeyDefinition& keyDefinition);

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

	KeyDefinition mKeyDefinition;
};

} // namespace application