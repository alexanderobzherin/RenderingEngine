# Project Packaging Guide

## Overview

Once your rendering application is ready for distribution, you can package it using:
```bash
python3 package_project.py
python3 package_project.py --packed
python3 package_project.py --packed --keep-plain
```

Before packaging, ensure that the project is built in Release mode.
If only a Debug build exists, the packager will use it automatically, assuming this is intentional.
Normally, however, applications should be shipped in Release mode for better performance and smaller binary size.

The purpose of packaging is to gather:

- the compiled application executable,
- all required assets,
- the Rendering Engine runtime library,
- and platform-dependent shared libraries

into a single, self-contained folder.
This folder can then be uploaded to platforms like Steam, or wrapped into an installer using tools such as:

- WiX Toolset — https://github.com/wixtoolset/wix/releases
- CPack (CMake) — https://cmake.org/cmake/help/latest/module/CPack.html

or delivered in any other custom way.

## Rationale

The packaging system of the Rendering Engine is intentionally aligned with Steam’s content structure requirements, which are practical, consistent, and easy for developers to follow.

Useful links from Steam’s official documentation:

- https://partner.steamgames.com/doc/home
- https://partner.steamgames.com/doc/sdk/uploading
- https://partner.steamgames.com/doc/store/review_process

These pages describe how application content should be organized and uploaded, and they serve as a reference for our packaging approach.

## Workflow

After packaging, the output appears under:

```bash
Build/UserApplications/<ApplicationName>/Packages/
```

The package has the following directory structure:

```bash
└───<ApplicationName>-v<X.Y.Z>-<Platform>
    ├───Binaries    --> executable + required shared libraries
    ├───Config      --> application configuration files
    ├───Content     --> assets (plain or packed)
    └───Logs        --> empty folder for runtime logs
```

A ZIP (or .tar.gz on Unix) archive of the package is also generated.

## Packaging Modes

1. Plain content (default)

```bash
python3 package_project.py
```

Assets remain in their normal directory structure under ```Content/```:

```bash
Content/
   Textures/
   Models/
   Shaders/
   Fonts/
```

2. Packed content

```bash
python3 package_project.py --packed
```

All assets are packed into:
```bash
Content/Pack.bin
Content/Pack.json
```

The engine reads assets directly from the packed file at runtime.
This mode produces the cleanest and smallest distribution.

3. Packed + keep plain
```bash
python3 package_project.py --packed --keep-plain
```
Both the packed file and the original asset folders are included.
The engine always prioritizes the packed version, but the plain files remain available for debugging, inspection, or comparison.
This mode is helpful when validating asset loading or debugging packaging issues.

<- [Back to Developer Guide Page](developer_guide.md)