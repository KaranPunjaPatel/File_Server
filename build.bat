@REM @echo off
@REM mkdir build 2>nul
@REM cd build
@REM cmake .. -G "MinGW Makefiles"
@REM cmake --build .
@REM cd ..

@REM build\File_Server.exe


@echo off
echo Building project...
mkdir build 2>nul
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build .
cd ..

:: Run one of the executables
echo Running Client...
build\Client\Client.exe

:: Or, uncomment the next line to run ServerApp instead
echo Running Server...
build\Server\Server.exe
