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
cd ..

:: Run the ClientApp (uncomment the appropriate line)
echo Running ClientApp...
.\build\src\Client\Client.exe

:: Or, uncomment the next line to run ServerApp instead
echo Running ServerApp...
.\build\src\Server\Server.exe
