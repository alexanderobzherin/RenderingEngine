:: This file is part of the Rendering Engine project.
:: Author: Alexander Obzherin <alexanderobzherin@gmail.com>
:: Copyright (c) 2026 Alexander Obzherin
:: Distributed under the terms of the zlib License. See LICENSE.md for details.

@echo off
setlocal

:: Project root is always the directory containing this script.
for %%I in ("%~dp0.") do set "PROJECT_SOURCE_DIR=%%~fI"
set "PROJECT_LOCAL_CONFIG=%PROJECT_SOURCE_DIR%\Local\rendering_engine.cmake"

:: An explicitly registered project owns its standalone Build directory.
if exist "%PROJECT_LOCAL_CONFIG%" (
    set "PROJECT_BUILD_DIR=%PROJECT_SOURCE_DIR%\Build"
    echo Registered project configuration found.
    goto :build
)

:: No explicit registration. Look for an enclosing Rendering Engine
:: workspace to preserve the legacy in-tree project layout.
set "SEARCH_DIR=%PROJECT_SOURCE_DIR%"

:search_loop
if not defined SEARCH_DIR goto :standalone_fallback

if exist "%SEARCH_DIR%\RenderingEngine\" if exist "%SEARCH_DIR%\UserApplications\" (
    set "ROOT_DIR=%SEARCH_DIR%"
    set "PROJECT_BUILD_DIR=%SEARCH_DIR%\Build\UserApplications\DynamicSpawnDestroy"
    echo Rendering Engine workspace found at: %SEARCH_DIR%
    goto :build
)

for %%I in ("%SEARCH_DIR%\..") do set "PARENT_DIR=%%~fI"

if not defined PARENT_DIR goto :standalone_fallback
if /I "%PARENT_DIR%"=="%SEARCH_DIR%" goto :standalone_fallback

set "SEARCH_DIR=%PARENT_DIR%"
goto :search_loop

:standalone_fallback
echo Rendering Engine workspace not found in parent directories.
echo Using project-local Build directory.
set "PROJECT_BUILD_DIR=%PROJECT_SOURCE_DIR%\Build"
goto :build

:build
echo Project source directory: %PROJECT_SOURCE_DIR%
echo Project build directory: %PROJECT_BUILD_DIR%

if exist "%PROJECT_BUILD_DIR%" (
    rmdir /s /q "%PROJECT_BUILD_DIR%"
    if errorlevel 1 exit /b 1
)

cmake -S "%PROJECT_SOURCE_DIR%" -B "%PROJECT_BUILD_DIR%" -A x64
if errorlevel 1 exit /b 1

endlocal
