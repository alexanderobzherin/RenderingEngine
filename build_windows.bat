
::Clean previously created binaries
rmdir /s /q Build
::Create new build directory and step into it
mkdir Build
cd Build
::Run root cmake file
set TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% -A x64
