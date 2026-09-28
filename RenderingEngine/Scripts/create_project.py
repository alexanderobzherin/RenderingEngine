#!/usr/bin/env python3
# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
##
# @file create_project.py
# @brief Rendering Engine demo/test application generator.
#
# This script creates a new Rendering Engine user application by copying the
# template folder structure and exposing template variables.
#
# By default, the project is created inside UserApplications/ and registered
# in UserApplications/CMakeLists.txt. An optional destination path can be
# provided to create and register an external UserApplication.
#
# Usage:
#   python3 create_project.py <ProjectName> [DestinationPath]
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

from register_project import register_project

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

def create_project(name, destination=DEST_ROOT):
    """
    @brief Create a new Rendering Engine user application.

    Generates the project folder structure and copies template files.
    Projects created in the default UserApplications directory are
    automatically registered in UserApplications/CMakeLists.txt.

    @param name         Name of the project to create.
    @param destination  Directory in which the project will be created.
    """
    destination = os.path.abspath(destination)
    default_destination = os.path.abspath(DEST_ROOT)
    project_dir = os.path.join(destination, name)

    if os.path.exists(project_dir):
        print(f"Error: Project '{name}' already exists at '{project_dir}'.")
        return

    os.makedirs(project_dir)
    copy_template(TEMPLATE_ROOT, project_dir, name)

    if destination == default_destination:
        file_path = os.path.join(default_destination, "CMakeLists.txt")
        cmake_add_project = "\n" + "add_subdirectory(" + name + ")"

        with open(file_path, "a") as cmake_file:
            cmake_file.write(cmake_add_project)
    else:
        if not register_project(project_dir):
            print(
                f"Warning: Project '{name}' was created successfully, "
                "but could not be registered."
            )
            return

    print(f"Created new project at {project_dir}")

if __name__ == "__main__":
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: create_project.py <ProjectName> [DestinationPath]")
        sys.exit(1)
    destination = sys.argv[2] if len(sys.argv) == 3 else DEST_ROOT
    create_project(sys.argv[1], destination)
