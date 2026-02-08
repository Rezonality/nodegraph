@echo off

echo %Time%

if not exist "vcpkg\vcpkg.exe" (
    cd vcpkg
    call bootstrap-vcpkg.bat -disableMetrics
    cd %~dp0
)

cd vcpkg
echo Installing Libraries
vcpkg install pkgconf tinyfiledialogs tinydir cppcodec ableton-link concurrentqueue tinydir kissfft portaudio stb date fmt clipp tomlplusplus glm sdl3[vulkan] magic-enum catch2 --triplet x64-windows-static-md --recurse
cd %~dp0
echo %Time%

