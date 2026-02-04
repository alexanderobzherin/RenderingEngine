:: This file is part of the Rendering Engine project.
:: Author: Alexander Obzherin <alexanderobzherin@gmail.com>
:: Copyright (c) 2026 Alexander Obzherin
:: Distributed under the terms of the zlib License. See LICENSE.md for details.

:: This file is part of the Rendering Engine project.
:: Author: Alexander Obzherin <alexanderobzherin@gmail.com>
:: Copyright (c) 2026 Alexander Obzherin
:: Distributed under the terms of the zlib License. See LICENSE.md for details.

@echo off
setlocal

set PROJECT_SOURCE_DIR=%cd%
set ROOT_DIR=%PROJECT_SOURCE_DIR%

:search_loop
if exist "RenderingEngine\" (
    set ROOT_DIR=%cd%
    goto :end
)

cd ..
goto :search_loop

:end
echo RenderingEngine found at location: %ROOT_DIR%
echo Project source direcroty: %PROJECT_SOURCE_DIR%

set PROJECT_BUILD_DIR=%ROOT_DIR%\Build\UserApplications\RenderSpriteMesh

rmdir /s /q %PROJECT_BUILD_DIR%
mkdir %PROJECT_BUILD_DIR%
cd %PROJECT_BUILD_DIR%

cmake -S "%PROJECT_SOURCE_DIR%" -B "%PROJECT_BUILD_DIR%" -A x64 -DRE_DEV_MODE=OFF

endlocal
