:: This file is part of the Rendering Engine project.
:: Author: Alexander Obzherin <alexanderobzherin@gmail.com>
:: Copyright (c) 2026 Alexander Obzherin
:: Distributed under the terms of the zlib License. See LICENSE.md for details.

@echo off
setlocal

set BUILD_ENGINE_ONLY=OFF
set BUILD_SDK=OFF
set BUILD_UNIT_TESTS=OFF
set BUILD_CONFIG=Debug

:parse_args
IF "%~1"=="" GOTO args_done

IF "%~1"=="--docs-only" (
    echo Generating Doxygen documentation only...
    if exist Build\Docs rmdir /s /q Build\Docs
    mkdir Build\Docs
    cd Build\Docs
    doxygen ..\..\Doxyfile
    exit /b %ERRORLEVEL%
)

IF "%~1"=="--build-sdk" (
    set BUILD_SDK=ON
    set BUILD_ENGINE_ONLY=ON
    set BUILD_CONFIG=Release
    SHIFT
    GOTO parse_args
)

IF "%~1"=="--build-unit-tests" (
    set BUILD_UNIT_TESTS=ON
    set BUILD_ENGINE_ONLY=OFF
    set BUILD_CONFIG=Debug
    SHIFT
    GOTO parse_args
)

echo Unknown argument: %~1
exit /b 1

:args_done

::Clean previously created binaries
rmdir /s /q Build
::Create new build directory and step into it
mkdir Build
cd Build
::Run root cmake file
set TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% -DVCPKG_TARGET_TRIPLET=x64-windows-static-md -A x64 -DRE_BUILD_ENGINE_ONLY=%BUILD_ENGINE_ONLY%
IF ERRORLEVEL 1 exit /b %ERRORLEVEL%

IF "%BUILD_UNIT_TESTS%"=="ON" (
    echo Building Rendering Engine unit tests...

    cmake --build . ^
        --config %BUILD_CONFIG% ^
        --target RenderingEngine_gtests

    IF ERRORLEVEL 1 exit /b %ERRORLEVEL%

    echo Unit test binaries built successfully.
    exit /b 0
)

IF "%BUILD_SDK%"=="ON" (
    cmake --build . --config Release
    IF ERRORLEVEL 1 exit /b %ERRORLEVEL%

    cmake --install . --config Release --prefix "Installed"
    IF ERRORLEVEL 1 exit /b %ERRORLEVEL%

    python3 ..\RenderingEngine\Scripts\package_sdk.py
    IF ERRORLEVEL 1 exit /b %ERRORLEVEL%

    exit /b 0
)
