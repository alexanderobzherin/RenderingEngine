# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.

# Load machine-local external UserApplication registrations.
set(RE_REGISTERED_PROJECTS_FILE
    "${CMAKE_CURRENT_LIST_DIR}/../Local/registered_projects.cmake"
)

if(EXISTS "${RE_REGISTERED_PROJECTS_FILE}")
    include("${RE_REGISTERED_PROJECTS_FILE}")

    foreach(APP_PATH IN LISTS RE_EXTERNAL_APPLICATIONS)
        get_filename_component(APP_NAME "${APP_PATH}" NAME)

        add_subdirectory(
            "${APP_PATH}"
            "${CMAKE_BINARY_DIR}/UserApplications/${APP_NAME}"
        )
    endforeach()
endif()
