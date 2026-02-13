#!/usr/bin/env python3
# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
##
# @file create_project.py
# @brief Rendering Engine demo/test application generator.
#
# This script creates a new project inside TestApplications/ by copying the
# template folder structure, exposing template variables, and registering
# the project in TestApplications/CMakeLists.txt.
#
# Usage:
#   python3 create_project.py <ProjectName>
#
# Platform notes:
#   - On Unix systems, .sh files copied from the template are given +x permission.
#   - On Windows, permissions remain unchanged.
#
# @author
#   Alexander Obzherin
#

import os
import sys
import shutil

TEMPLATE_ROOT = os.path.join("RenderingEngine", "Scripts", "Templates")
DEST_ROOT = os.path.join("UserApplications")

## @cond INTERNAL
def copy_template(src_dir, dst_dir, project_name):
    for root, dirs, files in os.walk(src_dir):
        rel_path = os.path.relpath(root, src_dir)
        target_root = os.path.join(dst_dir, rel_path)
        os.makedirs(target_root, exist_ok=True)

        for file in files:
            src_file = os.path.join(root, file)
            dst_file = os.path.join(target_root, file)
            # Do not process .in files inside Scripts/Templates
            if file.endswith(".in") and not rel_path.startswith(os.path.join("Scripts", "Templates")):
                dst_file = os.path.join(target_root, file.replace(".in", ""))

                with open(src_file, "r") as f:
                    content = f.read().replace("@PROJECT_NAME@", project_name)

                with open(dst_file, "w") as f:
                    f.write(content)
            else:
                shutil.copyfile(src_file, dst_file)
                
            if dst_file.endswith(".sh") and os.name == 'posix':
                import stat
                st = os.stat(dst_file)
                os.chmod(dst_file, st.st_mode | stat.S_IEXEC)
## @endcond

def create_project(name):
    """
    @brief Create a new Rendering Engine test/demo project.
    
    Generates the folder structure, copies template files, and appends
    'add_subdirectory()' to TestApplications/CMakeLists.txt.
    
    @param name  Name of the project to create.
    """
    project_dir = os.path.join(DEST_ROOT, name)
    if os.path.exists(project_dir):
        print(f"Error: Project '{name}' already exists.")
        return

    os.makedirs(project_dir)
    copy_template(TEMPLATE_ROOT, project_dir, name)
    FILE_PATH = os.path.join(DEST_ROOT, "CMakeLists.txt")
    CMAKE_ADD_PROJECT = "\n" + "add_subdirectory(" + name + ")"
    with open(FILE_PATH, "a") as cmake_file:
        cmake_file.write(CMAKE_ADD_PROJECT)
    print(f"Created new project at {project_dir}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: create_project.py <ProjectName>")
        sys.exit(1)
    create_project(sys.argv[1])