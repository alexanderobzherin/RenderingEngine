:: Written by Alexander Obzherin

:: Check for --docs-only argument
IF "%1"=="--docs-only" (
    echo Generating Doxygen documentation only...
    if exist Build\Docs rmdir /s /q Build\Docs
    mkdir Build\Docs
    cd Build\Docs
    doxygen ..\..\Doxyfile
    exit /b
)

::Clean previously created binaries
rmdir /s /q Build
::Create new build directory and step into it
mkdir Build
cd Build
::Run root cmake file
set TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% -A x64

:: Generate documentation
:: mkdir Docs
:: cd Docs
:: doxygen ..\..\Doxyfile
:: cd ..