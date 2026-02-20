#!/usr/bin/env python3
# This file is part of the Rendering Engine project.
# Author: Alexander Obzherin <alexanderobzherin@gmail.com>
# Copyright (c) 2026 Alexander Obzherin
# Distributed under the terms of the zlib License. See LICENSE.md for details.
##
"""
@file package_sdk.py
@brief Build-time helper for assembling the Rendering Engine SDK.

This script is intended to be invoked indirectly via the platform build
scripts:

  - Windows: build_engine.bat --build-sdk
  - Unix/FreeBSD: ./build_engine.sh --build-sdk

Responsibilities:
  - Read engine version from Intermediate/Generated/version.h.
  - Copy the installed RenderingEngine library, headers, and CMake config
    into a distributable SDK layout.
  - Copy vendored headers (glm, boost, nlohmann_json, Vulkan headers if used).
  - Copy project templates and the create_project.py helper.
  - Copy and patch ContentExamples into SDK-user mode (RE_DEV_MODE=OFF).
  - Emit documentation files and a human-readable Manifest.txt.
  - Create a compressed archive suitable for CI/CD artifacts.
"""

import os
import re
import shutil
import datetime
import logging
import sys

# --------------------------------------------------------------------
# Configuration
# --------------------------------------------------------------------

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../"))
BUILD_ROOT = os.path.join(REPO_ROOT, "Build")
INSTALL_ROOT = os.path.join(BUILD_ROOT, "Installed")
PACKAGE_ROOT = os.path.join(BUILD_ROOT, "Packages")
SDK_TEMP_ROOT = os.path.join(BUILD_ROOT, "SDK")
LOGS_ROOT = os.path.join(BUILD_ROOT, "Logs")

VERSION_HEADER = os.path.join(
    BUILD_ROOT, "Intermediate", "Generated", "version.h"
)

DOC_FILES = [
    "README.md",
    "Doc/app_config_guide.md",
    "Doc/application_development_guide.md",
    "Doc/ci_cd_guide.md",
    "Doc/debugging_guide.md",
    "Doc/developer_guide.md",
    "Doc/docker_container.md",
    "Doc/getting_started.md",
    "Doc/performance_profiling_guide.md",
    "Doc/prepare_environment.md",
    "Doc/project_creation_guide.md",
    "Doc/project_packaging_guide.md",
    "Doc/sdk_packaging_guide.md",
]

# --------------------------------------------------------------------
# Helpers
# --------------------------------------------------------------------


def read_version():
    """Parse version.h and extract MAJOR.MINOR.PATCH as a string."""
    if not os.path.isfile(VERSION_HEADER):
        raise RuntimeError(f"version.h not found at: {VERSION_HEADER}")

    with open(VERSION_HEADER, "r", encoding="utf-8") as f:
        text = f.read()

    major = re.search(r"#define\s+RENDERING_ENGINE_VERSION_MAJOR\s+(\d+)", text)
    minor = re.search(r"#define\s+RENDERING_ENGINE_VERSION_MINOR\s+(\d+)", text)
    patch = re.search(r"#define\s+RENDERING_ENGINE_VERSION_PATCH\s+(\d+)", text)

    if not (major and minor and patch):
        raise RuntimeError(
            "version.h missing required version macros "
            "(RENDERING_ENGINE_VERSION_MAJOR/MINOR/PATCH)."
        )

    return f"{major.group(1)}.{minor.group(1)}.{patch.group(1)}"


def detect_platform():
    """Return a human-readable platform string."""
    p = sys.platform
    if p.startswith("win"):
        return "Windows"
    elif p.startswith("linux"):
        return "Linux"
    elif p.startswith("freebsd"):
        return "FreeBSD"
    return "Unknown"


def copy_tree(src, dst, ignore=None):
    if not os.path.exists(src):
        logging.warning(f"Source path does not exist, skipping: {src}")
        return
    if os.path.exists(dst):
        shutil.rmtree(dst)
    shutil.copytree(src, dst, ignore=ignore)
    logging.info(f"Copied tree: {src} -> {dst}")


def ensure_dir(path):
    os.makedirs(path, exist_ok=True)
    return path


def write_manifest(path, version, platform):
    with open(path, "w", encoding="utf-8") as f:
        f.write("Rendering Engine SDK Manifest\n")
        f.write("=====================================\n\n")
        f.write(f"Version : {version}\n")
        f.write(f"Platform: {platform}\n")
        f.write(f"Date    : {datetime.datetime.now()}\n\n")
        f.write("Layout:\n")
        f.write("- RenderingEngine/RenderingLibrary : core engine library + headers + cmake files\n")
        f.write("- RenderingEngine/MaterialCompiler : material compiler tool + local engine DLL\n")
        f.write("- RenderingEngine/Scripts          : project generation scripts + templates\n")
        f.write("- ContentExamples/                 : sample projects\n")
        f.write("- UserApplications/                : place for user-created apps\n")
        f.write("- Doc/                             : documentation\n")
    logging.info(f"Manifest written: {path}")

def patch_build_script_for_sdk(build_script_path):
    """Force RE_DEV_MODE=OFF inside build_project.sh"""
    if not os.path.isfile(build_script_path):
        return

    with open(build_script_path, "r", encoding="utf-8") as f:
        content = f.read()

    # Patch the cmake configuration line:
    # cmake -S "$PROJECT_SOURCE_DIR" -B "$PROJECT_BUILD_DIR" -DCMAKE_BUILD_TYPE=...
    content = re.sub(
        r'cmake\s+-S\s+"\$PROJECT_SOURCE_DIR"\s+-B\s+"\$PROJECT_BUILD_DIR"\s+-DCMAKE_BUILD_TYPE=\$BUILD_MODE',
        r'cmake -S "$PROJECT_SOURCE_DIR" -B "$PROJECT_BUILD_DIR" -DCMAKE_BUILD_TYPE=$BUILD_MODE -DRE_DEV_MODE=OFF',
        content
    )

    with open(build_script_path, "w", encoding="utf-8") as f:
        f.write(content)

    logging.info(f"Patched build_project.sh for SDK: {build_script_path}")
    
# --------------------------------------------------------------------
# Main Packaging Logic
# --------------------------------------------------------------------


def main():
    # Init logging
    ensure_dir(LOGS_ROOT)
    logging.basicConfig(
        filename=os.path.join(LOGS_ROOT, "packaging.log"),
        level=logging.INFO,
        format="%(asctime)s %(levelname)s: %(message)s",
    )
    logging.info("=== SDK Packaging Started ===")

    try:
        version = read_version()
        platform = detect_platform()

        sdk_name = f"RenderingEngine-v{version}-SDK-{platform}"
        sdk_root = os.path.join(SDK_TEMP_ROOT, sdk_name)

        # Clean previous SDK for this version/platform
        if os.path.exists(sdk_root):
            shutil.rmtree(sdk_root)
        ensure_dir(sdk_root)

        logging.info(f"Version : {version}")
        logging.info(f"Platform: {platform}")
        logging.info(f"SDK root: {sdk_root}")

        # Paths inside SDK
        sdk_rendering_engine_root = ensure_dir(os.path.join(sdk_root, "RenderingEngine"))
        sdk_render_lib_root = ensure_dir(os.path.join(sdk_rendering_engine_root, "RenderingLibrary"))
        sdk_material_compiler_root = ensure_dir(os.path.join(sdk_rendering_engine_root, "MaterialCompiler"))
        sdk_scripts_root = ensure_dir(os.path.join(sdk_rendering_engine_root, "Scripts"))

        # --------------------------------------------------------------
        # 1. RenderingLibrary: headers + libs + cmake files
        # --------------------------------------------------------------
        installed_engine_root = os.path.join(INSTALL_ROOT, "RenderingEngine")

        # RenderingLibrary
        installed_render_lib = os.path.join(installed_engine_root, "RenderingLibrary")

        copy_tree(
            os.path.join(installed_render_lib, "Include"),
            os.path.join(sdk_render_lib_root, "Include"),
        )

        copy_tree(
            os.path.join(installed_render_lib, "Library"),
            os.path.join(sdk_render_lib_root, "Library"),
        )

        # --------------------------------------------------------------
        # 2. MaterialCompiler (tool + its local DLL)
        # --------------------------------------------------------------
        installed_mc_root = os.path.join(installed_engine_root, "MaterialCompiler")
        copy_tree(installed_mc_root, sdk_material_compiler_root)

        # --------------------------------------------------------------
        # 3. External vendored headers (glm, boost, json, vulkan,...)
        # --------------------------------------------------------------
        installed_external_root = os.path.join(installed_engine_root, "External")
        copy_tree(installed_external_root, os.path.join(sdk_rendering_engine_root, "External"))

        # --------------------------------------------------------------
        # 4. Scripts (Templates)
        # --------------------------------------------------------------
        copy_tree(
            os.path.join(REPO_ROOT, "RenderingEngine", "Scripts", "Templates"),
            os.path.join(sdk_scripts_root, "Templates"),
        )

        # --------------------------------------------------------------
        # 4. Modify CMakeLists.txt.in inside SDK to set RE_DEV_MODE OFF
        # --------------------------------------------------------------
        cmake_template_path = os.path.join(sdk_scripts_root, "Templates", "CMakeLists.txt.in")

        # Always copy the Templates directory
        copy_tree(
            os.path.join(REPO_ROOT, "RenderingEngine", "Scripts", "Templates"),
            os.path.join(sdk_scripts_root, "Templates"),
        )

        # Always copy create_project.py
        create_project_src = os.path.join(REPO_ROOT, "RenderingEngine", "Scripts", "create_project.py")
        if os.path.isfile(create_project_src):
            shutil.copy2(create_project_src, os.path.join(sdk_scripts_root, "create_project.py"))
            logging.info("Copied script: create_project.py")
        else:
            logging.warning(f"create_project.py not found at {create_project_src}")

        # Modify RE_DEV_MODE default in CMakeLists template
        cmake_template_path = os.path.join(sdk_scripts_root, "Templates", "CMakeLists.txt.in")

        with open(cmake_template_path, "r", encoding="utf-8") as f:
            text = f.read()

        text = re.sub(
            r'option\(RE_DEV_MODE\s+"[^"]+"\s+ON\)',
            'option(RE_DEV_MODE "Build using RenderingEngine directly from source tree (Windows only)" OFF)',
            text
        )

        with open(cmake_template_path, "w", encoding="utf-8") as f:
            f.write(text)

        logging.info("Updated CMakeLists.txt.in to set RE_DEV_MODE=OFF")


        # --------------------------------------------------------------
        # 5. Documentation
        # --------------------------------------------------------------
        doc_dst = ensure_dir(os.path.join(sdk_root, "Doc"))
        for doc in DOC_FILES:
            src = os.path.join(REPO_ROOT, doc)
            if os.path.isfile(src):
                shutil.copy2(src, doc_dst)
                logging.info(f"Copied documentation: {src}")
            else:
                logging.warning(f"Documentation file missing, skipped: {src}")

        # --------------------------------------------------------------
        # 6. Content Examples (copied directly from root-level folder)
        # --------------------------------------------------------------
        examples_src_root = os.path.join(REPO_ROOT, "ContentExamples")
        examples_dst_root = ensure_dir(os.path.join(sdk_root, "ContentExamples"))

        def ignore_example(dirpath, names):
            ignore = []
            for n in names:
                if n in ("Build", "Intermediate", ".vs", ".vscode", "x64"):
                    ignore.append(n)
                elif n.endswith(".user") or n.endswith(".vcxproj") or n.endswith(".vcxproj.filters"):
                    ignore.append(n)
            return ignore

        if os.path.isdir(examples_src_root):
            copy_tree(examples_src_root, examples_dst_root, ignore=ignore_example)
            logging.info("Copied ContentExamples folder.")

            # Patch build_project.sh in each example
            for proj in os.listdir(examples_dst_root):
                proj_path = os.path.join(examples_dst_root, proj)
                if os.path.isdir(proj_path):
                    build_script = os.path.join(proj_path, "build_project.sh")
                    patch_build_script_for_sdk(build_script)
        else:
            logging.warning("No ContentExamples folder found.")

        # --------------------------------------------------------------
        # 7. UserApplications (empty placeholder)
        # --------------------------------------------------------------
        ensure_dir(os.path.join(sdk_root, "UserApplications"))

        # --------------------------------------------------------------
        # 7. Manifest
        # --------------------------------------------------------------
        write_manifest(os.path.join(sdk_root, "Manifest.txt"), version, platform)

        # --------------------------------------------------------------
        # 8. Create Archive
        # --------------------------------------------------------------
        ensure_dir(PACKAGE_ROOT)
        archive_path = os.path.join(PACKAGE_ROOT, sdk_name)

        # Clean old archive if exists
        for ext in (".tar.gz", ".zip", ".tgz"):
            old = archive_path + ext
            if os.path.exists(old):
                os.remove(old)

        shutil.make_archive(archive_path, "gztar", root_dir=SDK_TEMP_ROOT)
        logging.info(f"Archive created: {archive_path}.tar.gz")
        logging.info("=== SDK Packaging Complete ===")

    except Exception as e:
        logging.error(f"SDK packaging failed: {e}", exc_info=True)
        print(f"SDK packaging failed: {e}")


if __name__ == "__main__":
    main()
