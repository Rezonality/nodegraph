set CURRENT_DIR=%CD%
mkdir build > nul
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..\
cd "%CURRENT_DIR%"

ln -srfv build\compile_commands.json compile_commands.json

