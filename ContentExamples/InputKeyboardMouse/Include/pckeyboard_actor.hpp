// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include "actor_2d.hpp"
#include "input_event.hpp"
#include "key_definition.hpp"

#include <vector>
#include <string>
#include <glm/vec2.hpp>

using namespace rendering_engine;

namespace rendering_engine
{

}

namespace application
{
class PCKeyboardKeyActor;

class PCKeyboardActor : public Actor2D
{
public:
	PCKeyboardActor(Scene& scene);

	void Initialize() override;

	void Update(float deltaTime) override;

	glm::vec2 GetDimensions() const { return mDimensions; }
	glm::vec2 GetLayoutMin() const { return mLayoutMin; }

private:
	void ComposeKeyboard();
	void ComposeRow(
		const std::vector<KeyDefinition>& keys,
		const glm::vec2& origin);

	void AddKey(
		const KeyDefinition& definition,
		const glm::vec2& position);

	static float CalculateRowWidth(const std::vector<KeyDefinition>& keys);

private:
	// list of key contains corresponding properties per key: Key, TextLabel, Button scale,
	static const std::vector<KeyDefinition> sLineFunction;
	static const std::vector<KeyDefinition> sLine123;
	static const std::vector<KeyDefinition> sLineQWERTY;
	static const std::vector<KeyDefinition> sLineASDF;
	static const std::vector<KeyDefinition> sLineZXCV;
	static const std::vector<KeyDefinition> sLineSpace;

	static const std::vector<KeyDefinition> sLineSystem;
	static const std::vector<KeyDefinition> sLineNavigation1;
	static const std::vector<KeyDefinition> sLineNavigation2;
	static const std::vector<KeyDefinition> sLineArrows;

	static const std::vector<KeyDefinition> sLineNumpadTop;
	static const std::vector<KeyDefinition> sLineNumpad789;
	static const std::vector<KeyDefinition> sLineNumpad456;
	static const std::vector<KeyDefinition> sLineNumpad123;
	static const std::vector<KeyDefinition> sLineNumpad0;

	static const KeyDefinition sNumpadAdd;
	static const KeyDefinition sNumpadEnter;
	static const KeyDefinition sArrowUp;

	std::vector<PCKeyboardKeyActor*> mKeys;

	glm::vec2 mDimensions{ 0.0f };
	glm::vec2 mLayoutMin{ 0.0f };
	glm::vec2 mLayoutMax{ 0.0f };
	bool bHasLayoutBounds = false;
};

} // namespace application