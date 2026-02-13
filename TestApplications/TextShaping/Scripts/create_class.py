#!/usr/bin/env python3
# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
##

import os
import sys
import re

SUPPORTED_BASES = {
    "Actor": "actors_based",
    "Actor2D": "actor_2d_based",
    "Scene": "scene_based"
}


def pascal_to_snake(name):
    result = ""
    for i, c in enumerate(name):
        if c.isupper() and i != 0 and not name[i-1].isupper():
            result += "_"
        result += c
    return result.lower()


def main():
    if len(sys.argv) != 3:
        print("Usage: create_class.py <Actor|Actor2D|Scene> <ClassName>")
        sys.exit(1)

    base_type = sys.argv[1]
    class_name = sys.argv[2]

    if base_type not in SUPPORTED_BASES:
        print(f"Error: Unsupported base type '{base_type}'")
        print("Supported types: Actor, Actor2D, Scene")
        sys.exit(1)

    # Resolve paths independent of execution directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(script_dir, ".."))
    template_dir = os.path.join(script_dir, "Templates")
    include_dir = os.path.join(project_root, "Include")
    source_dir = os.path.join(project_root, "Source")

    if not os.path.isdir(include_dir) or not os.path.isdir(source_dir):
        print("Error: Include/ or Source/ directory not found.")
        print("Make sure you are inside a valid RenderingEngine project.")
        sys.exit(1)

    template_prefix = SUPPORTED_BASES[base_type]

    header_template = os.path.join(template_dir, template_prefix + ".hpp.in")
    source_template = os.path.join(template_dir, template_prefix + ".cpp.in")

    if not os.path.isfile(header_template) or not os.path.isfile(source_template):
        print("Error: Template files not found.")
        sys.exit(1)

    class_file_name = pascal_to_snake(class_name)

    header_output = os.path.join(include_dir, class_file_name + ".hpp")
    source_output = os.path.join(source_dir, class_file_name + ".cpp")

    if os.path.exists(header_output) or os.path.exists(source_output):
        print("Error: Class files already exist.")
        sys.exit(1)

    # Process header template
    with open(header_template, "r") as f:
        header_content = f.read()

    header_content = header_content.replace("@CLASS_NAME@", class_name)
    header_content = header_content.replace("@CLASS_FILE_NAME@", class_file_name)

    with open(header_output, "w") as f:
        f.write(header_content)

    # Process source template
    with open(source_template, "r") as f:
        source_content = f.read()

    source_content = source_content.replace("@CLASS_NAME@", class_name)
    source_content = source_content.replace("@CLASS_FILE_NAME@", class_file_name)

    with open(source_output, "w") as f:
        f.write(source_content)

    print(f"Successfully created class '{class_name}'")
    print(f"  Header: {header_output}")
    print(f"  Source: {source_output}")


if __name__ == "__main__":
    main()