# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2025 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

add_subdirectory(RenderingLibrary)
add_subdirectory(MaterialCompiler)

if(NOT RE_BUILD_ENGINE_ONLY)
add_subdirectory(Tests)
endif()