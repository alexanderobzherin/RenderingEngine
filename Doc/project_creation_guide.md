# Project Creation Guide

## Overview

This document explains how to create a new project using the Rendering Engine.

A project can be created in two ways:

- **SDK-User Mode** — when the engine is downloaded as a precompiled SDK from an artifact repository.
- **Dev Mode** — when a project is created inside the engine’s source tree during engine development.


During the development of the rendering engine, it is needed to implement multiple test and demo applications. These applications serve two purposes:

- Feature development and verification — to implement, test, and debug new rendering features.
- Demonstration — to showcase the engine’s capabilities in a visual and accessible way.

Rendering Engine supports automated creation of new demo/test applications using a Python script:
```bash
python3 RenderingEngine/Scripts/create_project.py <ProjectName>
```

A generated project contains:
- Minimal C++ files (main + start scene)
- Auto-copied assets (Shaders, Models, Textures, Fonts)
- A per-project build script for Unix (build_project.sh)
- Auto-registration in TestApplications/CMakeLists.txt

This system works on Windows and FreeBSD/Linux, but the build workflows differ slightly.

## Windows Workflow

1. Create a new project
```bash
PS E:\Development\rendering-engine> python3 RenderingEngine\Scripts\create_project.py HelloWinApp
```
This creates:
```bash
TestApplications\HelloWinApp\
```

2. Generate the Visual Studio solution (Dev Mode only)
```bash
PS E:\Development\rendering-engine> .\build_engine.bat
```

Visual Studio solution appears under:
```bash
Build\
```
3. Generate the Visual Studio solution (SDK-User Mode only)
```bash
PS E:\Development\rendering-engine> cd .\ContentExamples\HelloWinApp\
PS E:\Development\rendering-engine\ContentExamples\HelloWinApp> .\build_project.bat
```

Open it in Visual Studio and build normally.
Assets and shaders will be copied automatically.

## FreeBSD / Linux Workflow

1. Create new project
```bash
$ python3 RenderingEngine/Scripts/create_project.py HelloUnixApp
```

2. Build the engine first (Dev Mode only)
Required because test apps depend on the installed package.
```bash
$ ./build_engine.sh --engine-only
```

This installs:
```bash
Build/Installed/
    Include/
    Library/
        libRenderingEngine.a
        cmake/RenderingEngine/*.cmake
```

3. Build the project
```bash
$ cd TestApplications/HelloUnixApp
$ ./build_project.sh
```

4. Run the application
```bash
$ cd ../../Build/TestApplications/HelloUnixApp/Release
$ ./HelloUnixApp
```

<- [Back to Developer Guide Page](developer_guide.md)