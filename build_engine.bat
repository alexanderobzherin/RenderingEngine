:: This file is part of the Rendering Engine project.
:: Author: Alexander Obzherin <alexanderobzherin@gmail.com>
:: Copyright (c) 2025 Alexander Obzherin
:: Distributed under the terms of the zlib License. See LICENSE.md for details.

:: Check for --docs-only argument
IF "%1"=="--docs-only" (
    :: Generate documentation
    echo Generating Doxygen documentation only...
    if exist Build\Docs rmdir /s /q Build\Docs
    mkdir Build\Docs
    cd Build\Docs
    doxygen ..\..\Doxyfile
    exit /b
)

IF "%1"=="--build-sdk" (
set BUILD_ENGINE_ONLY="ON"
)

::Clean previously created binaries
rmdir /s /q Build
::Create new build directory and step into it
mkdir Build
cd Build
::Run root cmake file
set TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -A x64 -DRE_BUILD_ENGINE_ONLY=%BUILD_ENGINE_ONLY%

IF "%1"=="--build-sdk" (
cmake --build . --config Release
cmake --install . --config Release --prefix "Installed"
)
