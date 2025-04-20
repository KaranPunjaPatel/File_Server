@echo off
echo Performing clean build...

:: Clean and prepare build directory
rd /s /q build
mkdir build
cd build

:: Run cmake to configure the project and build it
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build .

:: Go back to the root directory
@REM cd ..

:: Run the ClientApp (uncomment the appropriate line)
echo Running Client...
START "Client" .\src\Client\Debug\Client.exe

:: Or, uncomment the next line to run ServerApp instead
echo Running Server...
START "Server" .\src\Server\Debug\Server.exe
