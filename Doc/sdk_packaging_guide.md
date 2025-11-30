# SDK Packaging Guide

## Overview

The Rendering Engine can be used in two modes:

- **Dev Mode (Developer Mode)** — used when developing the engine itself, extending rendering features, or creating additional tools.
- **SDK-User Mode** — used when building graphics applications on top of the engine without modifying the engine source.

Generally, SDK packaging is part of the CI/CD pipeline.  
The result is a precompiled SDK stored under:
```bash
/Build/SDK/RenderingEngine-v<X.Y.Z>-SDK-<Platform>
```
and archived version under:
```bash
/Build/Packages/RenderingEngine-v<X.Y.Z>-SDK-<Platform>.tar.gz
```

## How to build the SDK

### Windows

Execute the script:
```bash
.\build_engine.bat --build-sdk
```

### FreeBSD/Linux

Execute the script:
```bash
./build_engine.sh --build-sdk
```

## SDK Folder Layout

A packaged SDK has the following structure:
```bash
└───RenderingEngine-v<X.Y.Z>-SDK-<Platform>
    ├───ContentExamples
    ├───Doc
    ├───RenderingEngine
    │   ├───External
    │   ├───MaterialCompiler
    │   ├───RenderingLibrary
    │   │   ├───Include
    │   │   └───Library
    │   │       └───cmake
    │   │           └───RenderingEngine
    │   └───Scripts
    │       └───Templates
    │           ├───Assets
    │           │   ├───Fonts
    │           │   ├───Models
    │           │   ├───Shaders
    │           │   └───Textures
    │           ├───Config
    │           ├───Include
    │           └───Source
    └───UserApplications
```

### Description of SDK Directories

##### ContentExamples
Contains example applications demonstrating engine features.
These projects link against the engine library and include additional build steps such as shader compilation.

##### Doc
Contains the engine documentation.

##### RenderingEngine
Root folder with all engine-related components.

##### RenderingEngine/External
Headers of third-party vendor libraries (GLM, Boost, JSON, etc.).

##### RenderingEngine/MaterialCompiler
A tool that reads material definitions, generates shader source code, and initiates shader compilation.

##### RenderingEngine/RenderingLibrary/Include
Public interface headers of the engine.

##### RenderingEngine/RenderingLibrary/Library
Compiled engine library and associated CMake config files.

##### RenderingEngine/Scripts
Project templates and script tools used to generate new applications.

##### UserApplications
A placeholder directory where SDK users can store their own projects.

### Current Restrictions and Future Improvements

SDK-user applications must currently be built in Release mode to match the precompiled engine.
Debug mode is supported only in Dev Mode (source-tree builds).

More flexibility (e.g., debug-build SDK) is planned for a future milestone.


<- [Back to Developer Guide Page](developer_guide.md)