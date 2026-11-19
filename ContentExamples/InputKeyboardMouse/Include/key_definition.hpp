
// This file is part of the Rendering Engine project.
// Author: Alexander Obzherin <alexanderobzherin@gmail.com>
// Copyright (c) 2026 Alexander Obzherin
// Distributed under the terms of the zlib License. See LICENSE.md for details.

#pragma once

#include <string>
#include <glm/glm.hpp>
#include "input_event.hpp"
#include <optional>

using namespace rendering_engine;

namespace application
{

struct KeyDefinition
{
	Key key = Key::Unknown;
	std::string label = "";
	glm::vec2 scale = {1.0f, 1.0f};
	std::optional<unsigned int> fontSize = std::nullopt;
};

} // namespace application