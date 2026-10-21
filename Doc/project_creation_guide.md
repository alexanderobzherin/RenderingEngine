# Project Creation Guide

## Overview

This document explains how to create a new project using the Rendering Engine.

A project can be created in two modes:

- **SDK-User Mode** - when an application uses the precompiled Rendering Engine SDK.
- **Dev Mode** - when an application uses the Rendering Engine source tree during engine development.

New projects can be created inside the Rendering Engine directory or in an external location, allowing applications to be maintained in their own repositories.

From the root directory of the engine or SDK, run:
```bash
python3 RenderingEngine/Scripts/create_project.py <ProjectName> [ProjectDirectory]
```

If `ProjectDirectory` is omitted, the project is created in the default `UserApplications/` directory.

A generated project contains:
- Minimal C++ application sources
- Assets and configuration files
- Project-specific build and packaging scripts

The created project is automatically registered with the current engine or SDK.

**Registering an Existing Project**

An existing external application can be registered with another Rendering Engine source tree or precompiled SDK.

From the root directory of the engine or SDK you want to use, run:

```bash
python3 RenderingEngine/Scripts/register_project.py /path/to/projects/HelloApp
```

The project can then be built against that engine or SDK installation.

To switch between engine installations, register the project again using the ```register_project.py``` script from the desired installation.

This system works on Windows and FreeBSD/Linux, but the build workflows differ slightly.

## Windows Workflow

1. Create a new project
```bash
PS E:\Development\rendering-engine> python3 RenderingEngine\Scripts\create_project.py HelloWinApp
```
This creates:
```bash
UserApplications\HelloWinApp\
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
PS E:\Development\rendering-engine> cd .\UserApplications\HelloWinApp\
PS E:\Development\rendering-engine\UserApplications\HelloWinApp> .\build_project.bat
```

Open it in Visual Studio and build normally.
Assets and shaders will be copied automatically.

## FreeBSD / Linux Workflow

1. Create new project
```bash
$ python3 RenderingEngine/Scripts/create_project.py HelloUnixApp
```

2. Build the engine first (Dev Mode only)
Required because applications depend on the installed engine.
```bash
$ ./build_engine.sh --engine-only [--debug | --release]
```

This installs:
```bash
Build/Installed/RenderingEngine/RenderingLibrary/
    Include/
    Library/
        libRenderingEngine.so
        cmake/RenderingEngine/*.cmake
```

3. Build the project
```bash
$ cd UserApplications/HelloUnixApp
$ ./build_project.sh [--debug | --release]
```

4. Run the application
```bash
$ cd ../../Build/UserApplications/HelloUnixApp/<Debug|Release>
$ ./HelloUnixApp
```
